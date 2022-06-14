local rotLabel

local function cubeUpdate(obj, dt)
    local pos = vec3(2, math.sin(1.5 * Client.getElapsedTime())*0.5+2, 0)
    obj:setPosition(pos)
    local rot = vec3(0, 1.5 * Client.getElapsedTime(), 0)
    obj:setEulerRotation(rot)
    rotLabel:setText("Cube pos: " .. vec2str(pos))
end

local cubeMdl = ModelManager.getModel("cube")
-- get scene
local scene = SceneManager.getActiveScene()
-- add floor
local floorObj = StaticMesh.new()
floorObj:setModel(cubeMdl)
floorObj:setScale(vec3(1000, 1, 1000))
scene:addObject(floorObj)
-- add cube
local cubeObj = StaticMesh.new()
cubeObj:setModel(cubeMdl)
cubeObj:onUpdate(cubeUpdate)
scene:addObject(cubeObj)

rotLabel = Label.new()
UI.addElement(rotLabel)
