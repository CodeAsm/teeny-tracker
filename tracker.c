#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include "libxmp-4.6.2/include/xmp.h"

#define SAMPLE_RATE 44100
#define CHANNELS 2  // Stereo output

xmp_context ctx = NULL;
snd_pcm_t *pcm_handle = NULL;
volatile bool keep_running = true;
int volume = 100;  // Initial volume (0-100)
volatile bool paused = false;

void cleanup() {
    if (pcm_handle) {
        snd_pcm_drain(pcm_handle);
        snd_pcm_close(pcm_handle);
        pcm_handle = NULL;
    }
    if (ctx) {
        xmp_end_player(ctx);
        xmp_release_module(ctx);
        xmp_free_context(ctx);
        ctx = NULL;
    }
}

void signal_handler(int signum) {
    keep_running = false;
    if (pcm_handle) {
        snd_pcm_drop(pcm_handle);  // Immediately stop the ALSA stream
    }
}

void set_terminal_mode(bool enable) {
    static struct termios oldt, newt;
    if (enable) {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);  // Set non-blocking mode
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, 0);  // Restore blocking mode
    }
}

void handle_input() {
    char ch;
    if (read(STDIN_FILENO, &ch, 1) == 1) {
        if (ch == '[') {
            volume = (volume > 0) ? volume - 5 : 0;
            xmp_set_player(ctx, XMP_PLAYER_VOLUME, volume);
        } else if (ch == ']') {
            volume = (volume < 100) ? volume + 5 : 100;
            xmp_set_player(ctx, XMP_PLAYER_VOLUME, volume);
        } else if (ch == ' ') {
            paused = !paused;
            if (paused) {
                xmp_stop_module(ctx);
            } else {
                xmp_restart_module(ctx);
            }
        }
    }
}

void play_module(const char *filename) {
    ctx = xmp_create_context();
    if (xmp_load_module(ctx, filename) != 0) {
        fprintf(stderr, "Error loading module: %s\n", filename);
        cleanup();
        exit(1);
    }

    // Retrieve and print module information
    struct xmp_module_info mi;
    xmp_get_module_info(ctx, &mi);
    printf("Playing: %s\n", mi.mod->name);
    printf("Type: %s\n", mi.mod->type);
    printf("Number of channels: %d\n", mi.mod->chn);
    printf("Number of patterns: %d\n", mi.mod->pat);
    printf("Number of instruments: %d\n", mi.mod->ins);

    // Print additional module information
    printf("Position: %d\n", mi.mod->len);
    printf("Pattern: %d\n", mi.mod->pat);
    printf("Row: %d\n", mi.mod->trk);
    printf("Number of rows: %d\n", mi.mod->chn);
    printf("Frame: %d\n", mi.mod->spd);
    printf("Speed: %d\n", mi.mod->bpm);
    printf("BPM: %d\n", mi.mod->gvl);
    printf("Time: %d ms\n", mi.mod->xxo);
    printf("Total time: %d ms\n", mi.mod->xxp);
    printf("Frame time: %d us\n", mi.mod->xxi);
    printf("Buffer size: %d\n", mi.mod->xxs);
    printf("Total buffer size: %d\n", mi.mod->xxc);
    printf("Volume: %d\n", mi.mod->gvl);
    printf("Loop count: %d\n", mi.mod->xxp);
    printf("Virtual channels: %d\n", mi.mod->xxi);
    printf("Used virtual channels: %d\n", mi.mod->xxs);
    printf("Sequence: %d\n", mi.mod->xxc);

    if (xmp_start_player(ctx, SAMPLE_RATE, 0) != 0) {
        fprintf(stderr, "Error starting player\n");
        cleanup();
        exit(1);
    }

    // Open ALSA playback device
    snd_pcm_hw_params_t *params;
    int err;

    if ((err = snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        fprintf(stderr, "Error opening ALSA device: %s\n", snd_strerror(err));
        cleanup();
        exit(1);
    }

    // Configure ALSA hardware parameters
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle, params, CHANNELS);
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &(unsigned int){SAMPLE_RATE}, NULL);
    snd_pcm_hw_params(pcm_handle, params);

    set_terminal_mode(true);  // Enable non-canonical mode for keyboard input

    struct xmp_frame_info info;
    while (keep_running && xmp_play_frame(ctx) == 0) {
        xmp_get_frame_info(ctx, &info);

        // Print current stats
        printf("Position: %d, Pattern: %d, Row: %d, Time: %d ms, Total time: %d ms, Speed: %d, Volume: %d%%\r",
               info.pos, info.pattern, info.row, info.time, info.total_time, info.speed, volume);
        fflush(stdout);

        // Check if buffer has data
        if (info.buffer_size > 0) {
            // Send PCM data to ALSA for playback
            if (snd_pcm_writei(pcm_handle, info.buffer, info.buffer_size / (CHANNELS * sizeof(short))) < 0) {
                snd_pcm_prepare(pcm_handle);
            }
        }

        // Handle volume control and pause
        handle_input();

        if (info.loop_count > 0) break;
    }

    set_terminal_mode(false);  // Restore terminal mode
    cleanup();
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <module file>\n", argv[0]);
        return 1;
    }

    // Set up signal handler for SIGINT
    signal(SIGINT, signal_handler);

    play_module(argv[1]);
    return 0;
}
