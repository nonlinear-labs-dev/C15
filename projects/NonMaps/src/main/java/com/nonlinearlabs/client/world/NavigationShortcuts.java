package com.nonlinearlabs.client.world;

import java.util.ArrayList;

public class NavigationShortcuts {
    public class Entry {
        public String path;
        public Runnable call;
        public int id;
    }

    static int maxId = 0;

    private ArrayList<Entry> entries = new ArrayList<Entry>();

    public NavigationShortcuts() {
        test("select group env a parameter attack time", "seaatt");
        test("select group env a parameter attack time", "seatta");
        negTest("select bank peter work 03 preset angel in background 1", "selattack");
    }

    private void test(String path, String q) {
        Entry e = new Entry();
        e.path = path;
        assert (matches(e, q));
    }

    private void negTest(String path, String q) {
        Entry e = new Entry();
        e.path = path;
        assert (!matches(e, q));
    }

    public void add(String path, Runnable r) {
        Entry e = new Entry();
        e.path = path;
        e.call = r;
        e.id = maxId++;
        entries.add(e);
    }

    public void execute(int id) {
        int numEntries = entries.size();
        for (int i = 0; i < numEntries; i++) {
            Entry e = entries.get(i);
            if (e.id == id) {
                e.call.run();
                return;
            }
        }
    }

    public ArrayList<Entry> query(String in) {
        String q = in.toLowerCase();
        ArrayList<Entry> filtered = new ArrayList<Entry>();

        if (q.length() == 0)
            return filtered;

        int numEntries = entries.size();
        for (int i = 0; i < numEntries && filtered.size() < 50; i++) {
            Entry e = entries.get(i);
            if (matches(e, q))
                filtered.add(e);
        }

        filtered.sort((a, b) -> {
            return compare(a.path, b.path, q);
        });
        return filtered;
    }

    private boolean matches(Entry e, String query) {
        // fast sort out
        String path = e.path.toLowerCase();
        if (query.charAt(0) != path.charAt(0))
            return false;

        int qlen = query.length();
        int pos = 1;
        for (int i = 1; i < qlen; i++) {
            char qchar = query.charAt(i);
            pos = path.indexOf(qchar, pos);
            if (pos == -1)
                return false;
        }

        // closer look
        return path.matches(getRegex(query));
    }

    private String getRegex(String query) {
        String regex = "" + query.charAt(0);
        int qlen = query.length();
        for (int i = 1; i < qlen; i++) {
            char qchar = query.charAt(i);
            regex += "(" + qchar + "|(.* " + qchar + "))";
        }

        return regex + ".*";
    }

    private int compare(String a, String b, String query) {
        int len = query.length();

        assert (a.charAt(0) == b.charAt(0));
        assert (a.charAt(0) == query.charAt(0));

        int apos = 1;
        int bpos = 1;

        for (int i = 1; i < len; i++) {
            char c = query.charAt(i);
            apos = a.indexOf(c, apos);
            bpos = b.indexOf(c, bpos);

            if (apos != bpos)
                return apos - bpos;
        }
        return 0;
    }
}