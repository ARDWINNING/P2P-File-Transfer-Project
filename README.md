Team Members:
- Alexander Winning
- William Glynn
- Lucas Benedict
- Zachary Suero

Checkpoint break down?
1. Message protocol (handshake, encoding/decoding, configuration).
2. File management (bitfields, splitting and reconstructing files).
3. Peer communication logic (interest, requests, and piece exchanges).
4. Choking/unchoking algorithm (neighbor selection, download rates, timers).

## Build

This project targets Linux (the CISE machines) and does not build natively on Windows: it uses BSD sockets (`arpa/inet.h`, `htonl`/`ntohl`), which are only available on Linux or WSL.

- **Windows:** install WSL with `wsl --install`, then inside the WSL shell run `sudo apt install build-essential`.
- **Build:** run `make` from the repo root.
- **Run a peer:** `./peerProcess <peerID>`, where `<peerID>` is a peer ID listed in `PeerInfo.cfg`.
