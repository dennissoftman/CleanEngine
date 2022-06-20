local lightPosX, lightPosY, lightPosY
local lightColR, lightColG, lightColB
local bulbObj
local function updateLight(dt)
    local scene = SceneManager.getActiveScene()
    local newDir = vec3(lightPosX:getValue(), lightPosY:getValue(), lightPosZ:getValue())
    scene:setLightPosition(newDir, 0)

    local newCol = vec3(lightColR:getValue(), lightColG:getValue(), lightColB:getValue())
    scene:setLightColor(newCol, 0)
    bulbObj:setPosition(newDir)
end

local supplyMdl = ModelManager.getModel("supply_depot")

local scene = SceneManager.getActiveScene()

local supplyObj = StaticMesh.new()
supplyObj:setModel(supplyMdl)
supplyObj:setScale(vec3(0.25))
scene:addObject(supplyObj)

Client.onUpdate(updateLight)

scene:setLightCount(1)
scene:setLightPosition(vec3(-1, -1, -1), 0)
scene:setLightColor(vec3(1), 0)

bulbObj = StaticMesh.new()
bulbObj:setModel(ModelManager.getModel("bulb"))
scene:addObject(bulbObj)

-- -- -- -- -- -- -- -- -- -- --
lightPosX = SpinBox.new()
lightPosX:setLabel("X")
UI.addElement(lightPosX)
lightPosY = SpinBox.new()
lightPosY:setLabel("Y")
UI.addElement(lightPosY)
lightPosZ = SpinBox.new()
lightPosZ:setLabel("Z")
UI.addElement(lightPosZ)

lightColR = SpinBox.new()
lightColR:setLabel("R")
lightColR:setValue(1.0)
UI.addElement(lightColR)
lightColG = SpinBox.new()
lightColG:setLabel("G")
lightColG:setValue(1.0)
UI.addElement(lightColG)
lightColB = SpinBox.new()
lightColB:setLabel("B")
lightColB:setValue(1.0)
UI.addElement(lightColB)
