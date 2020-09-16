package com.nonlinearlabs.client.useCases;

import java.util.HashMap;
import java.util.Map;
import java.util.function.Consumer;

import com.google.gwt.animation.client.Animation;
import com.google.gwt.core.client.Scheduler;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.dataModel.editBuffer.PhysicalControlParameterModel;

public class AnimationManager {

    class ReturnAnimation extends Animation {
        private double start;
        private double end;
        private Consumer<Double> cb;

        public ReturnAnimation(PhysicalControlParameterModel p) {
            start = p.value.getQuantizedAndClipped(true);
            end = 0.0;
        }

        void startIn(int delay, Consumer<Double> cb) {
            this.cb = cb;
            Scheduler.get().scheduleFixedPeriod(() -> {
                run(1500);
                return false;
            }, delay);
        }

        @Override
        protected void onUpdate(double progress) {
            final double curve = 2;
            final double bent = 0.5 + Math.atan(2 * curve * progress - curve) / (2 * Math.atan(curve));
            cb.accept(start + bent * (end - start));
        }

        @Override
        protected void onComplete() {
            super.onComplete();
            cb.accept(end);
        }
    }

    private Map<ParameterId, Animation> runningAnimations = new HashMap<ParameterId, Animation>();
    private boolean recursionGuard = false;

    public void startReturnAnimation(PhysicalControlParameterModel p) {
        startDelayedAnimation(p, 0);
    }

    public void cancelAnimation(PhysicalControlParameterModel m) {
        if (recursionGuard)
            return;

        runningAnimations.remove(m.id);
    }

    public void startDelayedAnimation(PhysicalControlParameterModel p, int delay) {
        if (recursionGuard)
            return;

        runningAnimations.remove(p.id);

        ReturnAnimation animation = new ReturnAnimation(p);
        animation.startIn(delay, v -> {
            if (runningAnimations.get(p.id) == animation) {
                recursionGuard = true;

                EditBufferUseCases.get().setParameterValue(p.id, v, true);
                recursionGuard = false;
            }
        });

        runningAnimations.put(p.id, animation);
    }
}