-- get scene
local scene = SceneManager.getActiveScene()
local ammoLabel
local loadedLabel

local weaponObj
local weaponAnimComp
local weaponMdlName  = "shotgun"
-- animations
local weaponIdleAnim = ModelManager.getAnimation(weaponMdlName, "idle")
local weaponFireAnim = ModelManager.getAnimation(weaponMdlName, "fire")

local weaponReloadAnims = {
	['reload_one_in_empty'] = 1,
	['reload_two_in_empty'] = 1,
	['reload_one_in_one']   = 1,
	['reload_two_in_one']   = 1,
	['reload_two_in_one']   = 1,
	['reload_two_in_two']   = 1,
	['reload_one_to_one']   = 1,
	['reload_one_to_two']   = 1,
	['reload_empty_one']    = 1,
	['reload_empty_two']    = 1,
}

-- weapon state
local weaponAmmoCount   = 0
local weaponLoadedCount = 2
local weaponFiredCount  = 0

for k, v in pairs(weaponReloadAnims) do
	print("Loading animation "..k)
	weaponReloadAnims[k] = ModelManager.getAnimation(weaponMdlName, k)
end

local function updateAmmoLabel()
	ammoLabel:setText("Ammo: "..weaponAmmoCount)
end

local function updateLoadedLabel()
	loadedLabel:setText("Loaded "..weaponLoadedCount.."/2")
end

local function onMouseButton(btn, action)
	if btn == 0 and action == 1 then
		if weaponLoadedCount > 0 then
			weaponAnimComp:setAnimation(weaponFireAnim)
			if weaponAnimComp:trigger() then
				AudioManager.playSound("shotgun_fire", weaponObj:getPosition())
				weaponLoadedCount = weaponLoadedCount - 1
				weaponFiredCount = weaponFiredCount + 1
				updateLoadedLabel()
			end
		end
	end
end

local function onKeyAction(key, scancode, action, mods)
	if scancode == 19 and action == 1 then -- R key
		local targetAnim = nil
		
		local ammoToLoad = math.min(2-weaponLoadedCount, weaponAmmoCount)
		if ammoToLoad <= 0 then
			if weaponFiredCount < weaponLoadedCount then return end
		end

		local totalCount = weaponFiredCount + weaponLoadedCount

		if totalCount == 0 then
			if ammoToLoad == 1 then
				targetAnim = "reload_one_in_empty"
			else
				targetAnim = "reload_two_in_empty"
			end
		elseif totalCount == 1 then
			if weaponLoadedCount == 1 then
				targetAnim = "reload_one_to_one"
			elseif ammoToLoad == 0 then
				targetAnim = "reload_empty_one"
			elseif ammoToLoad == 1 then
				targetAnim = "reload_one_in_one"
			else
				targetAnim = "reload_two_in_one"
			end
		else
			if weaponLoadedCount == 1 then
				targetAnim = "reload_one_to_two"
			elseif ammoToLoad == 0 then
				targetAnim = "reload_empty_two"
			elseif ammoToLoad == 1 then
				targetAnim = "reload_one_in_two"
			else
				targetAnim = "reload_two_in_two"
			end
		end

		if targetAnim ~= nil then
			weaponAnimComp:setAnimation(weaponReloadAnims[targetAnim])
			if weaponAnimComp:trigger() then
				weaponFiredCount  = 0
				weaponLoadedCount = weaponLoadedCount + ammoToLoad
				weaponAmmoCount = weaponAmmoCount - ammoToLoad
				
				updateAmmoLabel()
				updateLoadedLabel()
			end
		end
	elseif scancode == 78 and action == 1 then
		weaponAmmoCount = weaponAmmoCount + 1
		updateAmmoLabel()
	elseif scancode == 74 and action == 1 then
		weaponAmmoCount = weaponAmmoCount - 1
		if weaponAmmoCount < 0 then weaponAmmoCount = 0 end
		updateAmmoLabel()
	end
	
	print("Scancode: "..scancode)
end

local cubeMdlId = ModelManager.getModelId("cube")
-- add floor
local floorObj = Entity()
local floorObjMesh = StaticMeshComponent(floorObj)
floorObjMesh:setModelId(cubeMdlId)
floorObj:attachComponent(floorObjMesh)
floorObj:setScale(vec3(1000, 1, 1000))
scene:addObject(floorObj)

-- add weapon

local weaponMdlId = ModelManager.getModelId(weaponMdlName)

weaponObj = Entity()
weaponAnimComp = AnimatedModelComponent(weaponObj)
weaponAnimComp:setModelId(weaponMdlId)
weaponObj:attachComponent(weaponAnimComp)
weaponAnimComp:setAnimation(weaponIdleAnim)
weaponObj:setPosition(vec3(0, 1, 0))
scene:addObject(weaponObj)

Input.onKeyboard(onKeyAction)
Input.onMouseButton(onMouseButton)

ammoLabel = Label()
updateAmmoLabel()
UI.addElement(ammoLabel)

loadedLabel = Label()
updateLoadedLabel()
UI.addElement(loadedLabel)
