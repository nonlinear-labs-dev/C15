package com.nonlinearlabs.client.world.pointer;

import com.nonlinearlabs.client.world.Position;

public class Touch {
    public Touch(int id, Position p) {
        this.id = id;
        this.pos = p;
    }

    public int id = 0;
    public Position pos = new Position();
}