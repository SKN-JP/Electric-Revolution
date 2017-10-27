machine = {
	name = "輸出ポート",
	size = {x = 5, y = 3},
	group = "外部接続ポート"
}

i = 0
j = 1
function updateSystem()

    if isItemPos(0, 1) then
        i = i + 1
        moveItem(0, 1, 0, 0.0625)
        if i == 4 then
            i = 0
            j = j + 1
            if j == 9 then
                j = 1
            end
        end
    end
    if isItemPos(1, 1) and isTradeTile(5, 1) then
        removeItem(1, 1)
    end

end

function draw()
    drawTexture("image.png")

    drawTextureAt("conveyor/conveyor"..j..".png", 1, 1, 0, 1)
end

function canPutItemAt(x, y, id)
    if ((x == 1 and y == 1) or (x == 0 and y == 1)) and isTradeTile(5, 1) then
        return true
    else
        return false
    end
end