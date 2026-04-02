# The Duck Song - Nintendo DS Visual Novel

## Prerequisites

- [devkitPro](https://devkitpro.org/wiki/Getting_Started) with devkitARM
- [melonDS](https://melonds.kuribo64.net/downloads.php) (or DeSmuME) for testing

## Installation

Clone the repository and build:

```bash
git clone https://github.com/YOUR_USERNAME/ds-visual-novel.git
cd ds-visual-novel
make
```

This produces `ds_visual_novel.nds` in the project root.

## Project Structure

Project structure should look like this after running 'make'.
```
ds_visual_novel/
├── .idea/
├── build/
├── source/
│   ├── game_assets/
│   └── main.c
├── ds_visual_novel.elf
├── ds_visual_novel.nds
├── Makefile
└── README.md
```

## Usage

1. compile your code:
    ```bash
    make
    ```

2. Open `ds_visual_novel.nds` in the melonDS application.

3. Tap the screen or press A to advance through the story.

<img width="260" height="386" alt="image" src="https://github.com/user-attachments/assets/c04978b4-68fb-4fd1-815f-c8647f367ee2" />


For a clean rebuild:

```bash
make clean
make
```
