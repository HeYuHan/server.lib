local a = A()
a = nil
json = require "json"

local server = Net.WebListenner()
server:Listen('0.0.0.0:9796',10)

local function OnClientConnected(client)
    print('client connected uid:' .. tostring(client.uid))
    client:Send(json.encode({key1="1234",key2=234,key3=true}))

end
local function OnClientMessage(client,msg)
    print('client uid:' .. tostring(client.uid) .. ' message => ' .. msg)
end
local function OnClientDisconnected(client)
    print('client disconnected uid:' .. tostring(client.uid))
end

local function OnServerAccept(client)
    print('accept client uid' .. tostring(client.uid))
    client:RegisterCallBack({
        OnConnected = OnClientConnected,
        OnDisconnected = OnClientDisconnected,
        OnMessage = OnClientMessage
    })
end



server:RegisterCallBack({OnAccept = OnServerAccept})