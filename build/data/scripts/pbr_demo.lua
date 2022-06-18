local function objUpdate(obj, dt)
    local rot = vec3(0, 0.35 * Client.getElapsedTime(), 0)
    obj:setEulerRotation(rot)
end

local bulbObj
local function lightUpdate(dt)
    local scene = SceneManager.getActiveScene()
    local et = Client.getElapsedTime()
    local newPos = vec3(math.sin(et), 1, math.cos(et))
    scene:setLightPosition(newPos, 0)
    bulbObj:setPosition(newPos)
end

local floorMdl = ModelManager.getModel("floor")
local bulbMdl  = ModelManager.getModel("bulb")
local scifiMdl = ModelManager.getModel("scifi-storage")
-- get scene
local scene = SceneManager.getActiveScene()
-- add floor
local floorObj = StaticMesh.new()
floorObj:setModel(floorMdl)
floorObj:setScale(vec3(0.5))
scene:addObject(floorObj)
Client.onUpdate(lightUpdate)
-- add container
local pbrObj = StaticMesh.new()
pbrObj:setModel(scifiMdl)
pbrObj:setPosition(vec3(0, 0.5, 0))
pbrObj:setScale(vec3(0.5))
pbrObj:onUpdate(objUpdate)
scene:addObject(pbrObj)
-- add "light bulb"
bulbObj = StaticMesh.new()
bulbObj:setModel(bulbMdl)
scene:addObject(bulbObj)
-- initialize light properties
scene:setLightCount(1)
scene:setLightColor(vec3(10), 0)
