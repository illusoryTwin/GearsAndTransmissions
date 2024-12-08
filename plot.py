import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV file
data = pd.read_csv('data/motor_data.csv')

# Create the plot
plt.plot(data['time (millisec)'], data['position_value (degree)'], label='Position vs. Time', color='blue')

# Add labels and a title
plt.xlabel('Time (ms)')
plt.ylabel('Position Value (°)')
plt.title('Rotor Position vs. Time')
plt.legend()

# Show the plot
plt.grid(True)
plt.show()
