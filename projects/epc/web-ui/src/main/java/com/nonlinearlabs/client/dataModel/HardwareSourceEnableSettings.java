package com.nonlinearlabs.client.dataModel;

public class HardwareSourceEnableSettings {
    public HardwareSourceEnableSettings(int w, int h) {
        m_data = new boolean[8][5];
    }

    static HardwareSourceEnableSettings fromString(String s) {
        HardwareSourceEnableSettings r = new HardwareSourceEnableSettings(8, 4);
        String hws[] = s.split("\n");

        for(int hw = 0; hw < 8; hw++) { 
            String[] os = hws[hw].split(",");
            boolean[] row = r.m_data[hw];

            for(int o = 0; o < 5; o++) {
                row[o] = os[o] == "1";
            }
        }

        return r;
    }

    @Override
    public String toString() {
        String ret = new String();
        for(boolean hw[] : m_data)
        {
          for(boolean value : hw)
          {
            ret += value ? "1," : "0,";
          }
          ret += "\n";
        }
        return ret;
    }

    public boolean[][] m_data;    
}