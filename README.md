A3 is to ASCII art as animated GIF is to GIF. This project defines a file format and provides a basic sample implementation.

# A3 file format

An A3 file consists of a four-byte **header** followed by a **payload**. The first two bytes of the header are the ASCII characters 'A' and '3'. The next two are reserved bytes which may be used for configuration in the future; in this first version, they must both be 0 (ASCII NUL).

The payload is a buffer compressed with `zlib` ([RFC 1950](http://tools.ietf.org/html/rfc1950) and [RFC 1951](http://tools.ietf.org/html/rfc1951)). When uncompressed, the first four bytes of this buffer are a 32-bit little-endian integer representing the **pause value** (that is, the amount of time paused between animation frames) in milliseconds. The remainder of the payload buffer is the animation frames themselves, in plain text, delimited by ASCII NUL characters. The buffer also ends in NUL.

The format does not specify a character encoding, and the implementation does not care; it blindly copies the bytes from your files. UTF-8 is obviously recommended.

# Implementation

The reference implementation has two component programs: `a3make` and `a3show`.

`a3make` will build an A3 file out of a series of files, the names of which are passed as command line arguments. The user may specify the pause value with the `-p` flag and the output filename with the `-o` flag. If not specified, the pause value will be 100 ms and the file will be output to standard output.

>     $ ls
>     frame01.txt  frame02.txt  frame03.txt  frame04.txt  frame05.txt
>     $ cat *
>     (´A` )
>     ( ´A`)
>     (  ´A)
>     (   ´)
>     (    )
>     $ a3make -o test.a3 -p 150 frame??.txt

`a3show` will display an A3 file. This requires a terminal (emulator) that understands ANSI escape codes; i.e. everything but the Windows defaults.

>     $ a3show test.a3
>     <animation>

If you pass more than one argument, it will play all files in succession.

## Compilation

You will need a POSIX system, a C compiler, `make`, and the [zlib](http://zlib.net/) library. On Debian, the packages `build-essential` and `zlib1g-dev` will give you everything you need.

Then, just run `make` in the repository's directory to build both executables.
