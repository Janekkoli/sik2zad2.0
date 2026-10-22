# Kierki

A small C++ multiplayer card game project built around a TCP server and four clients. The server loads game definitions from a text file, distributes cards, validates moves, tracks scores, and coordinates turns across players seated at N, E, S, and W.

This project is a networked implementation of the classic trick-taking card game known in Polish as "Kierki".

## Features

- Server-driven game flow with four connected players
- Text file based game setup and card definitions
- Support for IPv4 client connections
- Automatic player mode (`-a`) for bot-like clients
- Score and total-score reporting after each hand
- Sample game data and launch scripts in the `przykl/` folder

## Project structure

- `src/` – implementation files for the server, client, game logic, cards, and networking
- `include/` – headers for the core game and protocol structures
- `przykl/` – examples, test data, and helper scripts
- `CMakeLists.txt` – project build configuration
- `LICENSE` – project license

## Build

Requirements:

- CMake 3.20+
- C++23 compatible compiler
- Linux environment (the code uses POSIX socket APIs)

From the project root:

```bash
cmake -S . -B build
cmake --build build
```

This builds the following executables in the repository root:

- `kierki-serwer`
- `kierki-klient`
- `main`

## Run the server

The server requires a port and a game definition file. The file contains one or more game setups, each listing the game type and the cards for each player.

Example:

```bash
./kierki-serwer -p 2053 -f przykl/rozgrywka.txt -t 23
```

Arguments:

- `-p <port>` – server port
- `-f <file>` – game file path
- `-t <timeout>` – client inactivity timeout in seconds

## Run clients

Each client connects to the server, selects a seat, and can either be controlled manually or automatically.

Example with four AI players:

```bash
./kierki-klient -h localhost -p 2053 -N -4 -a
./kierki-klient -h localhost -p 2053 -E -4 -a
./kierki-klient -h localhost -p 2053 -S -4 -a
./kierki-klient -h localhost -p 2053 -W -4 -a
```

Arguments:

- `-h <host>` – server hostname or IP
- `-p <port>` – server port
- `-4` – IPv4 mode
- `-N | -E | -S | -W` – chosen table seat
- `-a` – automatic player mode

> The code also includes IPv6 support flags, but the active implementation currently prefers IPv4 in the client entry point.

## Game file format

The sample files in `przykl/` are plain text game definitions. Each game starts with a game type and first player, followed by four hands.

Example:

```text
1N
2C3C4C5C6C7C8C9C10CJCQCKCAC
2D3D4D5D6D7D8D9D10DJDQDKDAD
2H3H4H5H6H7H8H9H10HJHQHKHAH
2S3S4S5S6S7S8S9S10SJSQSKSAS
```

Interpretation:

- `1` = game type / deal type
- `N` = first player in the game
- Each next line contains the 13-card hand for a seat: N, E, S, W
- Cards are encoded in a compact format such as `2C`, `10D`, `QC`, `AS`

## Example workflow

1. Start the server on a chosen port.
2. Connect one client for each seat: `N`, `E`, `S`, `W`.
3. The server assigns deals and begins the first trick.
4. Clients send valid card plays using the protocol.
5. The server checks legality, updates scores, and sends new trick/deal information.

A ready-made example script is included:

```bash
bash przykl/test1.sh
```

## Notes

- The project uses a custom binary/text protocol over sockets.
- The server is the source of truth for game validation and progression.
- The sample scripts and files under `przykl/` are useful for testing and manual verification.

## License

This repository is distributed under the MIT license. See `LICENSE` for details.
