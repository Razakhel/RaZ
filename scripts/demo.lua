local app   = Application.new()
local world = app:addWorld(3)

local render = world:addRenderSystem(1280, 720, "RaZ")

local window = render:getWindow()

window:addKeyCallback(Keyboard.ESCAPE, function () app:quit() end)
window:setCloseCallback(function () app:quit() end)

local camera = world:addEntity()
camera:addComponent(Transform.new(Vec3f.new(0, 0, 5)))
camera:addComponent(Camera.new(window:getWidth(), window:getHeight()))

local shield      = world:addEntity()
local shieldTrans = shield:addComponent(Transform.new(Vec3f.new(), Quaternionf.identity(), Vec3f.new(0.1)))
local _, shieldRendererData = ObjFormat.load(FilePath.new(RAZ_ROOT .. "assets/meshes/shield.obj"))
shield:addComponent(shieldRendererData)

local cube      = world:addEntity()
local cubeTrans = cube:addComponent(Transform.new(Vec3f.new(), Quaternionf.identity(), Vec3f.new(0.5)))
cube:addComponent(MeshRenderer.new(Mesh.new(AABB.new(Vec3f.new(-1), Vec3f.new(1)))))
  :getMaterials()[1]:getProgram():setAttribute(ColorPreset.Red:toVec(), MaterialAttribute.BaseColor)

local sphere      = world:addEntity()
local sphereTrans = sphere:addComponent(Transform.new(Vec3f.new(2, 0, 0)))
sphere:addComponent(MeshRenderer.new(Mesh.new(Sphere.new(Vec3f.new(0), 0.5), 10, SphereMeshType.UV)))
  :getMaterials()[1]:getProgram():setTexture(Texture2D.create(ColorPreset.Green), MaterialTexture.BaseColor)

local light = world:addEntity()
light:addComponent(Transform.new())
light:addComponent(Light.new(LightType.DIRECTIONAL, -Axis.Z, 1, ColorPreset.White))

local globalTime = 0
app:run(function (deltaTime)
    globalTime = globalTime + deltaTime

    shieldTrans:rotate(Radiansf.new(Degreesf.new(90 * deltaTime)), Vec3f.new(1, 1, 1):normalize())
    cubeTrans.position  = Vec3f.new(-2, math.sin(globalTime * 3), 0)
    sphereTrans.scaling = Vec3f.new(math.sin(globalTime * 2) * 0.25 + 0.75)
end)
