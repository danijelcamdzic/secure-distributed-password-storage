import paho.mqtt.client as mqtt
import time
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.backends import default_backend

# Load the RSA public key from the PEM file
def load_public_key(filename):
    with open(filename, "rb") as key_file:
        public_key = serialization.load_pem_public_key(
            key_file.read(), backend=default_backend()
        )
    return public_key

# Encrypt the message using the RSA public key
def encrypt_message(message, public_key):
    encrypted_message = public_key.encrypt(
        message,
        padding.PKCS1v15()
    )
    return encrypted_message

# Callback function for when the client receives a CONNACK response from the server
def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")

    # Subscribe to the desired topic
    client.subscribe("/topic/topic2")

# Callback function for when a PUBLISH message is received from the server
def on_message(client, userdata, msg):
    print(f"{msg.topic}: {msg.payload.decode()}")

# Set up the MQTT client
client = mqtt.Client(protocol=mqtt.MQTTv311)
client.on_connect = on_connect
client.on_message = on_message

# Connect to the MQTT broker
client.connect("mqtt.eclipseprojects.io", 1883, 120)

# Load the RSA public key
public_key_hardware_node_1 = load_public_key("public_key_hardware_node_1.pem")

# Start the MQTT client loop
client.loop_start()

# Wait a bit
time.sleep(3)

try:
    while True:
        # Get user input for the message to send
        message = input("Enter the message to send: ")

        # Encrypt the message using the RSA public key
        encrypted_message = encrypt_message(message.encode(), public_key_hardware_node_1)

        # Get the number of bytes in the encrypted message
        num_bytes = len(encrypted_message)

        # Print the number of bytes
        print(f"The encrypted message has {num_bytes} bytes.")

        # Add delimiter to distinguish end message
        end_message = b'END_MESSAGE'
        encrypted_message_with_end = encrypted_message + end_message

        # Send the encrypted message to the desired topic
        client.publish("/topic/topic3", encrypted_message_with_end)

except KeyboardInterrupt:
    # Stop the MQTT client loop and disconnect
    client.loop_stop()
    client.disconnect()
    print("\nDisconnected from the MQTT broker.")

