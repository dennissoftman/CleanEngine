local function hostFunction()
    Server.create("*:25541") -- create and join
    Client.connect("localhost:25541")
end

local targetIP = "192.168.1.62"
local function joinFunction()
    Client.connect(targetIP..":25541")
end

hostBtn = Button.new()
hostBtn:setText("Host")
hostBtn:onClick(hostFunction)
UI.addElement(hostBtn)

joinBtn = Button.new()
joinBtn:setText("Join")
joinBtn:onClick(joinFunction)
UI.addElement(joinBtn)
