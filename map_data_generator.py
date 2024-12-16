import csv

def generate_empty_map_csv(w, h, output_file):
    """
    Generate a CSV file representing an empty tile-based map of width w and height h.
    """
    # Initialize the map_data with empty spaces (0)
    map_data = [[0 for _ in range(w)] for _ in range(h)]

    # Write the map data to the CSV file
    with open(output_file, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerows(map_data)

    return output_file

# Set map dimensions and output file
w = 25  # Width
h = 19  # Height
output_file = "./tile_based_map_dynamic.csv"

# Generate the CSV file
generate_empty_map_csv(w, h, output_file)
