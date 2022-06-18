local lblPtr, flip, spinVal = nil, false, 0.0
local function beepFunction()
    if flip then
        lblPtr:setText("beep "..(spinVal))
        flip = false
    else
        lblPtr:setText("bop "..(spinVal))
        flip = true
    end
end

local function changeFunction(a)
    spinVal = a
end

local btnA = Button.new()
btnA:setText("Work in")
btnA:onClick(beepFunction)
UI.addElement(btnA)

local btnB = Button.new()
btnB:setText("Progress")
btnB:onClick(beepFunction)
UI.addElement(btnB)

-- Don't ask me why slider is spinbox
-- I will fix that l8r
local spinA = SpinBox.new()
spinA:setMinimum(-10)
spinA:setMaximum(10)
spinA:onChangeValue(changeFunction)
UI.addElement(spinA)

lblPtr = Label.new()
lblPtr:setText("beep")
UI.addElement(lblPtr)

Client.releaseCursor()
