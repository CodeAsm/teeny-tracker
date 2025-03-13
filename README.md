# Teeny-Tracker Music Player

Teeny-Tracker is a music player for tracker music files. It is currently under development and works on Linux.

## Getting Started

### Prerequisites

Make sure you have the following installed:
- Git
- GCC
- Make

### Building

1. Clone the repository and initialize submodules:
    ```sh
    git clone --recurse-submodules https://gitlab.com/codeasm/teeny-tracker.git
    cd teeny-tracker
    ```

2. Build the project using Make:
    ```sh
    make
    ```

### Usage

Run the music player:
```sh
./tracker <module file>
```

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request.

## Acknowledgements

- Thanks to the creators of libxmp for their invaluable creation.
- Special thanks to the tracker music community.

## LICENSE

Teeny-Tracker Player
Copyright (C) 2025 Nico Vijlbrief or also known as CodeAsm

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
