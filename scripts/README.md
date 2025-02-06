## Setup

1. Install Python3
2. In the `scripts` directory, create a virtual environment with the name `.venv`: `python3 -m venv ./.venv/`
3. Activate the virtual environment: `source .venv/bin/activate` (MacOS/Linux), `.venv/Scripts/activate.ps1` (Windows)
4. Install required libraries: `pip install -r requirements.txt`
5. Deactivate the virtual environment when finished: `deactivate`

## Running the script

Run the script using `python paint.py` to retexture every png file in `data/textures/`. The new textures are output in destination directory `data/retextures/`. Learn more about using different palettes by trying `python paint.py -h`.
