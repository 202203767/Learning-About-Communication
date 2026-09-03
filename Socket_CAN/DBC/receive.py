import can
import cantools

DB = cantools.database.load_file("can.dbc")

bus = can.interface.Bus (channel="vcan0", interface="socketcan")

print("waiting")

while True:
    message = bus.recv()
    try:
        DBC_meg = DB.get_message_by_frame_id(message.arbitration_id)
        decoded = DBC_meg.decode(message.data)
        print(decoded)
    except Exception as e:
        print(e)