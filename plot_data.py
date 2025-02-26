import pandas as pd
import matplotlib.pyplot as plt
import sys


def plot_data(filename):
    # Load the CSV file into a pandas DataFrame
    df = pd.read_csv(filename)
    # Check if the necessary columns exist
    if 'Time' not in df.columns or 'Glycemia' not in df.columns or 'Insulin Infusion' not in df.columns:
        print("CSV file must contain 'Time', 'Glycemia', and 'Insulin Infusion' columns.")
        return

    # Plot Time vs. Glucose Level with thinner lines (no markers)
    plt.figure(figsize=(10, 5))
    plt.plot(df['Time'], df['Glycemia'], color='r', label='Glycemia',
             linewidth=1.5)  # Thinner lines without markers
    plt.xlabel('Time (minutes)')
    plt.ylabel('Glycemia (mg/dL)')
    plt.title('Time vs. Glucose Level')
    plt.grid(True)
    plt.legend()
    plt.savefig('glucose_level_plot.png')  # Save the plot to a file

    # Plot Time vs. Insulin Infusion Rate with thinner lines (no markers)
    plt.figure(figsize=(10, 5))
    plt.plot(df['Time'], df['Insulin Infusion'], color='b',
             label='Insulin Infusion', linewidth=1.5)  # Thinner lines without markers
    plt.xlabel('Time (minutes)')
    plt.ylabel('Insulin Infusion Rate (micro units / min)')
    plt.title('Time vs. Insulin Infusion Rate')
    plt.grid(True)
    plt.legend()
    plt.savefig('insulin_infusion_plot.png')  # Save the plot to a file

    # Close the plots to free up memory
    plt.close('all')


def main():
    # Use 'glucose_data.csv' as the default filename if not provided as an argument
    filename = sys.argv[1] if len(sys.argv) > 1 else 'glucose_data.csv'
    
    # Check if the file exists and is a CSV file
    if not filename.endswith('.csv'):
        print("Error: The file must be a CSV file.")
        return
    
    try:
        plot_data(filename)
    except FileNotFoundError:
        print(f"Error: The file '{filename}' was not found.")
    except pd.errors.EmptyDataError:
        print(f"Error: The file '{filename}' is empty.")
    except pd.errors.ParserError:
        print(f"Error: The file '{filename}' could not be parsed.")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")


if __name__ == "__main__":
    main()
