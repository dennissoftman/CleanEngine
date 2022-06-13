local function cubeUpdate(obj, dt)
    obj:setPosition(vec3(2, math.sin(1.5 * Client.getElapsedTime())*0.5+2, 0))
    obj:setEulerRotation(vec3(0, 1.5 * Client.getElapsedTime(), 0))
end

cubeMdl = ModelManager.getModel("cube")
-- get scene
scene = SceneManager.getActiveScene()
-- add floor
floorObj = StaticMesh.new()
floorObj:setModel(cubeMdl)
floorObj:setScale(vec3(1000, 1, 1000))
scene:addObject(floorObj)
-- add cube
cubeObj = StaticMesh.new()
cubeObj:setModel(cubeMdl)
cubeObj:onUpdate(cubeUpdate)
scene:addObject(cubeObj)
