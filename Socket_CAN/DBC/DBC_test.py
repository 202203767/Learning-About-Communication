import cantools

db = cantools.database.load_file("can.dbc")

msg = db.get_message_by_frame_id(0x100)

decoded = msg.decode(bytes([0xB8, 0x04])) #リトルエンディアン

print(decoded)