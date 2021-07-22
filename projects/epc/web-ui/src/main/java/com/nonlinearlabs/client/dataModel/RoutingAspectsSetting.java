package com.nonlinearlabs.client.dataModel;

import com.google.gwt.core.client.GWT;

public class RoutingAspectsSetting {
    
    public static int numAspects = 5;
    public static int numRoutings = 10;

    public RoutingAspectsSetting() {
        m_data = new boolean[numRoutings][numAspects];
    }

    static RoutingAspectsSetting fromString(String s) {
        RoutingAspectsSetting r = new RoutingAspectsSetting();
        
        String splitRoutingsCommaSeperatedList[] = s.split("\n");

        for(int routing = 0; routing < numRoutings; routing++) { 
            String[] splitOnesAndZeros = splitRoutingsCommaSeperatedList[routing].split(",");
            
            boolean[] row = new boolean[numAspects];
            for(int aspect = 0; aspect < numAspects; aspect++) {
                row[aspect] = splitOnesAndZeros[aspect] == "1";
            }
            r.m_data[routing] = row;
        }

        int sum = 0;
        for(boolean[] row: r.m_data) {
            for(boolean entry: row) {
                sum += entry ? 1 : 0;
            }
        }

        GWT.log("sum of all bools: " + sum);

        return r;
    }

    @Override
    public String toString() {
        String ret = new String();
        for(boolean routing[] : m_data)
        {
          for(boolean value : routing)
          {
            ret += value ? "1," : "0,";
          }
          ret += "\n";
        }
        return ret;
    }

    public boolean[][] m_data;    
}