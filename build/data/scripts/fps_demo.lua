keyboardButtons = {0, 0, 0, 0}
cursorLocked = true

function keyHandler(key, scancode, action, mods)

    if key == 256 and action > 0 then
        if cursorLocked then
            Client.releaseCursor()
            cursorLocked = false
        else
            Client.lockCursor()
            cursorLocked = true
        end
        return
    end

    if key == 87 then
        if action < 0 then keyboardButtons[1] = 0
        elseif action > 0 then keyboardButtons[1] = 1 end
    elseif key == 83 then
        if action < 0 then keyboardButtons[2] = 0
        elseif action > 0 then keyboardButtons[2] = 1 end
    end

    if key == 65 then
        if action < 0 then keyboardButtons[3] = 0
        elseif action > 0 then keyboardButtons[3] = 1 end
    elseif key == 68 then
        if action < 0 then keyboardButtons[4] = 0
        elseif action > 0 then keyboardButtons[4] = 1 end
    end
end

camMoveSpeed   = 2
camRotateSpeed = 10
cameraRotation = vec3(math.rad(45.0), 0, 0)
function mouseHandler(mx, my)
    if not mouseLastPos then
        mouseLastPos = vec2(mx, my)
        return
    end
    scene = SceneManager.getActiveScene()
    cam = scene:getCamera()
    
    dt = Client.getDeltaTime()
    dx = mx - mouseLastPos.x
    dy = my - mouseLastPos.y

    xrot = 0
    if dy > 0 then
        xrot = camRotateSpeed*dt
    elseif dy < 0 then
        xrot = -camRotateSpeed*dt
    end
    oldx = cameraRotation.x
    cameraRotation.x = cameraRotation.x + xrot
    if math.abs(cameraRotation.x) > math.pi/2 then
        cameraRotation.x = oldx
    end

    yrot = 0
    if dx > 0 then
        yrot = -camRotateSpeed*dt
    elseif dx < 0 then
        yrot = camRotateSpeed*dt
    end
    cameraRotation.y = cameraRotation.y + yrot
    
    cam:setEulerRotation(cameraRotation)
    
    mouseLastPos = vec2(mx, my)
end

function updateHandler(dt)
    cam = SceneManager.getActiveScene():getCamera()
    dv = vec3(0, 0, 0)
    if keyboardButtons[1] > 0 then
        dv = dv + cam:front() * camMoveSpeed * dt
    elseif keyboardButtons[2] > 0 then
        dv = dv - cam:front() * camMoveSpeed*dt
    end

    if keyboardButtons[3] > 0 then
        dv = dv + cam:right() * camMoveSpeed*dt
    elseif keyboardButtons[4] > 0 then
        dv = dv - cam:right() * camMoveSpeed*dt
    end
    cam:setPosition(cam:getPosition() + dv)
end


Client.onUpdate(updateHandler)
Input.onKeyboard(keyHandler)
Input.onMouseMove(mouseHandler)

cube_model = ModelManager.getModel("cube")
scene = SceneManager.getActiveScene()
-- create floor
floorObj = StaticMesh.new()
floorObj:setModel(cube_model)
floorObj:setScale(vec3(1000, 1, 1000))
scene:addObject(floorObj)
-- create object
obj = StaticMesh.new()
obj:setModel(cube_model)
obj:setPosition(vec3(0, 1, 1))

scene:addObject(obj)

Client.lockCursor()
