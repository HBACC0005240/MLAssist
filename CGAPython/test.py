import CGAPython
import time


cga=CGAPython.CGA()
print(cga.IsConnected())
print(cga.IsInGame())
print(cga.GetWorldStatus())
print(cga.GetGameStatus())
print(cga.GetBGMIndex())
cga.Connect(4396)
print(cga.IsConnected())

msg = CGAPython.cga_chat_msg_t()
def charCallBack(msg):
	print(msg.unitid)
	print(msg.msg)
	print(msg.color)
	print(msg.size)
cga.RegisterChatMsgNotify(charCallBack)
print(cga.IsInGame())
print(cga.GetWorldStatus())
print(cga.GetGameStatus())
print(cga.GetBGMIndex())
#cga.GetWorldStatus()
#cga.GetGameStatus()
#cga.GetBGMIndex()
time.sleep(100)