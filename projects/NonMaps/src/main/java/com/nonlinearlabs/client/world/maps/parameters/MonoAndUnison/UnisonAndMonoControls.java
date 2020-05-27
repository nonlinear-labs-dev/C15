package com.nonlinearlabs.client.world.maps.parameters.MonoAndUnison;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.LayoutResizingHorizontal;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.Spacer;

class UnisonAndMonoControls extends LayoutResizingHorizontal {

   	UnisonAndMonoControls(MapsLayout parent) {
        super(parent);
        addChild(new Spacer(this, 2, 2));
        addChild(new MonoControls(this));
        addChild(new UnisonControls(this));
        addChild(new Spacer(this, 2, 2));
    }

    public void fillBackground(Context2d ctx)
	{
		getPixRect().drawRoundedRect(ctx, Rect.ROUNDING_ALL, toXPixels(6), toXPixels(2), new Gray(26), null);
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		fillBackground(ctx);
		super.draw(ctx, invalidationMask);
	}

    @Override
    public Control doubleClick(Position pos) {
       return null;
    }

    @Override
    public RGB getColorModuleBackground() {
        return new Gray(87);
    }
     
    @Override
    public void doFirstLayoutPass(double levelOfDetail) {
        super.doFirstLayoutPass(levelOfDetail);
    }
}
