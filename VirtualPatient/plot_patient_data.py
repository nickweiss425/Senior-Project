import matplotlib.pyplot as plt
import pandas as pd

# Read the CSV file
data = pd.read_csv('patient_data.csv')

# Plot the data
plt.figure(figsize=(10, 6))
plt.plot(data['time'], data['glucose_level'], label='Glucose Level')
plt.xlabel('Time (minutes)')
plt.ylabel('Glucose Level (mg/dL)')
plt.title('Patient Glucose Level Over Time')
plt.legend()
plt.grid(True)
plt.show()
