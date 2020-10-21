package com.nonlinearlabs.client.world.maps.presets.html;

import com.google.gwt.dom.client.Element;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.maps.NonRect;

public class PositionMappers {
    public static native double getElementLeft(Element element)
    /*-{
       var rectObject = element.getBoundingClientRect();
       return rectObject.left;
    }-*/;

    public static native double getElementTop(Element element)
    /*-{
       var rectObject = element.getBoundingClientRect();
       return rectObject.top;
    }-*/;

    public static native double getElementWidth(Element element)
    /*-{
       var rectObject = element.getBoundingClientRect();
       return rectObject.width;
    }-*/;

    public static native double getElementHeight(Element element)
    /*-{
       var rectObject = element.getBoundingClientRect();
       return rectObject.height;
    }-*/;

    public static double clientXToNonX(double x) {
        NonRect viewPortRect = NonMaps.get().getNonLinearWorld().getViewport().getNonPosition();
        double z = NonMaps.get().getNonLinearWorld().getCurrentZoom();
        double vpx = z * viewPortRect.getLeft() / NonMaps.devicePixelRatio;
        return NonMaps.devicePixelRatio * (vpx + x) / z;
    }

    public static double clientYToNonY(double y) {
        NonRect viewPortRect = NonMaps.get().getNonLinearWorld().getViewport().getNonPosition();
        double z = NonMaps.get().getNonLinearWorld().getCurrentZoom();
        double vpy = z * viewPortRect.getTop() / NonMaps.devicePixelRatio;
        return NonMaps.devicePixelRatio * (vpy + y) / z;
    }

    public static double clientDimensionToNonDimension(double d) {
        double z = NonMaps.get().getNonLinearWorld().getCurrentZoom();
        return NonMaps.devicePixelRatio * d / z;
    }

    public static NonRect getElementsNonRect(Element e) {
        return new NonRect(clientXToNonX(getElementLeft(e)), clientYToNonY(getElementTop(e)),
                clientDimensionToNonDimension(getElementWidth(e)), clientDimensionToNonDimension(getElementHeight(e)));
    }

}