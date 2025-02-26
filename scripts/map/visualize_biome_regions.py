import numpy as np
import matplotlib.pyplot as plt
from param_based_biome import BIOME_IDS, ID_TO_BIOME, classify_biome, Biome, classify_temperature, classify_humidity, Weirdness
from constants import DATA_PATH

def visualize_biome_regions(resolution=200):
    temp_values = np.linspace(-1, 1, resolution)
    hum_values = np.linspace(-1, 1, resolution)

    weirdness_categories = {
        Weirdness.NORMAL: 0.0,
        Weirdness.UNUSUAL: 0.75,
        Weirdness.EXTREME: 0.95  # if you handle extremes
    }

    for weirdness_category, weirdness_val in weirdness_categories.items():
        biome_grid = np.zeros((resolution, resolution))

        for i, hum in enumerate(hum_values):
            for j, temp in enumerate(temp_values):
                biome_id = classify_biome(temp, hum, weirdness_val)
                biome_grid[resolution - i - 1, j] = biome_id

        # Generate color map
        cmap = plt.get_cmap('tab20', len(Biome))
        plt.figure(figsize=(8, 6))
        plt.imshow(biome_grid, cmap=cmap, interpolation='nearest')

        # Create meaningful colorbar
        cbar = plt.colorbar(ticks=np.arange(len(Biome)))
        cbar.ax.set_yticklabels([ID_TO_BIOME[id] for id in np.arange(len(Biome))])

        plt.title(f'Biome Regions (Weirdness: {weirdness_category.value})')
        plt.xlabel('Temperature')
        plt.ylabel('Humidity')
        plt.xticks([0, resolution//2, resolution-1], ['Low (-1)', 'Mid (0)', 'High (1)'])
        plt.yticks([0, resolution//2, resolution-1], ['High (1)', 'Mid (0)', 'Low (-1)'])
        plt.xlabel("Temperature")
        plt.ylabel("Humidity")

        plt.savefig(f"{DATA_PATH}/maps/biome_region_{weirdness_category.value.lower()}.png", bbox_inches='tight', dpi=300)
        plt.close()
