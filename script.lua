-- default settings
defaults = {
    object_color = "#FFFFFF", -- set the curve color
    samples = 3000, -- default samples
    traversal = 2, -- default traversal mode number
    resolution = { height = 480, width = 720, keep_aspect = true; ask_for_resize = false; } -- default image loading resolution
}

local mapping1 = function ()
    print("HELLO WORLD FROM LUA")
end

-- Custom mapping
maps = {
    { name = "Mapping1", func = mapping1 },
}
