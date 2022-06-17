local keyboardButtons = {0, 0, 0, 0}
local cursorLocked = false

local function keyHandler(key, scancode, action, mods)

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

-- FPS properties
local camMoveSpeed   = 2
local camRotateSpeed = 1.6
local cameraRotation = vec3(math.rad(45.0), 0, 0)
--
local function mouseHandler(mx, my)
    if not mouseLastPos or not cursorLocked then
        mouseLastPos = vec2(mx, my)
        return
    end
    scene = SceneManager.getActiveScene()
    cam = scene:getCamera()
    
    dt = Client.getDeltaTime()
    dx = mx - mouseLastPos.x
    dy = my - mouseLastPos.y

    xrot = dy * camRotateSpeed * dt
    oldx = cameraRotation.x
    cameraRotation.x = cameraRotation.x + xrot
    if math.abs(cameraRotation.x) > math.pi/2 then
        cameraRotation.x = oldx
    end

    yrot = -dx * camRotateSpeed * dt
    cameraRotation.y = cameraRotation.y + yrot
    
    cam:setEulerRotation(cameraRotation)
    
    mouseLastPos = vec2(mx, my)
end

local function updateHandler(dt)
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

local function coinUpdater(obj, dt)
    obj:setEulerRotation(vec3(math.pi/2, (2 * Client.getElapsedTime()), 0))
    obj:setPosition(vec3(2, math.sin(2 * Client.getElapsedTime())*0.25 + 1.25, 0))
end

Client.onUpdate(updateHandler)
Input.onKeyboard(keyHandler)
Input.onMouseMove(mouseHandler)
-- Client.lockCursor()
