-- spawn object
function generate_colliseum(radius, height)
    if radius <= 0 or height <= 0 then return end
    -- save model pointer
    cubeMdl = ModelManager.getModel("cube")
    -- get scene
    scene = SceneManager.getActiveScene()
    -- add floor
    floorObj = StaticMesh.new()
    floorObj:setModel(cubeMdl)
    floorObj:setScale(vec3(1000, 1, 1000))
    scene:addObject(floorObj)
    -- precompute some values
    cLen = 2 * math.pi * radius
    slabsInRing = math.ceil(cLen * 0.45) 
    objScale = vec3(2, 1, 1)

    -- 
    objPos = vec3(0)
    objRot = vec3(0)
    flip = 0
    for i = 0, height-1 do
        objPos.y = i+1
        for j = 0, slabsInRing-1 do
            cPos = (j - flip*0.5) / slabsInRing
            cRot = cPos * 2 * math.pi
                        
            obj = StaticMesh.new()
            obj:setModel(cubeMdl)
            obj:setScale(objScale)
            -- set position
            objPos.x = math.sin(cRot) * radius
            objPos.z = math.cos(cRot) * radius
            obj:setPosition(objPos)
            -- set rotation
            objRot.y = cRot
            obj:setEulerRotation(objRot)
            -- TODO: register physics
            --
            scene:addObject(obj)
        end
        if flip > 0 then flip = 0 else flip = 1 end
    end
end
--

generate_colliseum(32, 16)
