package com.nonlinearlabs.client.world.maps.parameters.MonoAndUnison;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.LayoutResizingHorizontal;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.Spacer;

class UnisonAndMonoControls extends LayoutResizingHorizontal {

    private class Fill extends Spacer {

        public Fill(MapsLayout parent) {
            super(parent);
        }

        @Override
        public void draw(Context2d ctx, int invalidationMask) {
            Rect pix = getPixRect();
            pix.fill(ctx, getParent().getColorModuleBackground());
        }
    };

	UnisonAndMonoControls(MapsLayout parent) {
        super(parent);
        addChild(new Spacer(this, 2, 1));
        addChild(new MonoControls(this));
        addChild(new Fill(this));
        addChild(new UnisonControls(this));
        addChild(new Spacer(this, 2, 1));
    }
    
    @Override
    public void doFirstLayoutPass(double levelOfDetail) {
        super.doFirstLayoutPass(levelOfDetail);
    }
}
