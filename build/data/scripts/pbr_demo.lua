local function sphereUpdate(obj, dt)
    local rot = vec3(0, 0.35 * Client.getElapsedTime(), 0)
    obj:setEulerRotation(rot)
end

local function lightUpdate(dt)
    scene = SceneManager.getActiveScene()
    et = Client.getElapsedTime()
    local newPos = vec3(math.sin(et), 1, math.cos(et))
    scene:setLightPosition(newPos, 0)
end

local floorMdl = ModelManager.getModel("floor")
local sphereMdl = ModelManager.getModel("snowball")
-- get scene
local scene = SceneManager.getActiveScene()
-- add floor
local floorObj = StaticMesh.new()
floorObj:setModel(floorMdl)
scene:addObject(floorObj)
Client.onUpdate(lightUpdate)
-- add snowball
local sphereObj = StaticMesh.new()
sphereObj:setModel(sphereMdl)
sphereObj:setPosition(vec3(0, 0.5, 0))
sphereObj:setScale(vec3(0.25))
sphereObj:onUpdate(sphereUpdate)
scene:addObject(sphereObj)
