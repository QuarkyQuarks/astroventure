loadType("algine::Widget")
loadType("algine::Painter")
loadType("algine::RectF")
loadType("algine::Dimen")
loadType("algine::Unit")
loadType("algine::Paint")
loadType("algine::Color")

loadModule("SEColorScheme")
loadType("GameScene")

function onDrawBackground(self, painter)
    painter:setPaint(self.background)
    painter:drawRoundRect(RectF(0, 0, self.width, self.height), Dimen(8, Unit.dp):pixels())
end

function updateColorScheme(self)
    local scene = GameScene.cast(self:getParentScene())
    local scheme = scene:getColorSchemeManager():getColorScheme()
    self.background = Paint(scheme.btnBackground)
end
