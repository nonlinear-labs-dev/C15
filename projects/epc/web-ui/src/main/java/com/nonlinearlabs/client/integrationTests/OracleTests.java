package com.nonlinearlabs.client.integrationTests;

import com.google.gwt.core.client.GWT;
import com.google.gwt.user.client.DOM;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.ServerProxy;
import com.nonlinearlabs.client.WebSocketConnection;
import com.nonlinearlabs.client.dataModel.presetManager.Bank;
import com.nonlinearlabs.client.dataModel.presetManager.Banks;
import com.nonlinearlabs.client.world.maps.NonPosition;
import com.nonlinearlabs.client.world.maps.presets.PresetManager;

public class OracleTests extends TestWithSteps {

    String bankName = "Oracle-testbank-" + DOM.createUniqueId();
    WebSocketConnection webSocket;
    ServerProxy server = NonMaps.get().getServerProxy();
    PresetManager pm = NonMaps.get().getNonLinearWorld().getPresetManager();
    boolean documentReceived = false;
    int lastDocumentID = -1;

    OracleTests(Runnable after) {
        super(after);

        createWebSocket();

        disableDirectLoad();

        createBank();

        createPresets();

        selectPresetViaProxy();
        selectPresetViaNewWebSocket();

        deleteBank();

        start();
    }

    private void disableDirectLoad() {
        server.setSetting("DirectLoad", "off");
    }

    private void selectPresetViaNewWebSocket() {
        addStep(() -> {
            documentReceived = false;
            webSocket.send(
                    "/presets/banks/select-preset?uuid=" + findBank().presets.getValue().get(0) + "&isOracle=true");
        }, () -> {
            boolean wasOracleOfServer = server.lastDocumentCouldOmitOracles();
            return !wasOracleOfServer && documentReceived;
        });

        setStepName("Select Via WebSocket");
    }

    private void selectPresetViaProxy() {
        addStep(() -> {
            documentReceived = false;
            lastDocumentID = server.getLastUpdateID();
            server.selectPreset(findBank().presets.getValue().get(1));
        }, () -> {
            boolean wasOracleOfServer = server.lastDocumentCouldOmitOracles();
            boolean updateIsNewer = lastDocumentID < server.getLastUpdateID();
            GWT.log("update was Oracle: " + wasOracleOfServer + " is newer: " + updateIsNewer + " received on ws: "
                    + documentReceived);
            return wasOracleOfServer && documentReceived && updateIsNewer;
        });

        setStepName("Select Via Proxy");
    }

    private void createWebSocket() {
        webSocket = new WebSocketConnection();
        webSocket.connectToServer(new WebSocketConnection.ServerListener() {

            @Override
            public void onServerUpdate(String text) {
                documentReceived = true;
            }

            @Override
            public void onServerConnectionOpened() {
            }
        });
    }

    private void createBank() {
        addStep(() -> {
            server.newBank(bankName, new NonPosition());
        }, () -> {
            return findBank() != null;
        });

        setStepName("Create Bank");
    }

    private void createPresets() {
        addStep(() -> {
            Bank b = findBank();
            server.appendEditBuffer(b.uuid.getValue());
            server.appendEditBuffer(b.uuid.getValue());
            server.appendEditBuffer(b.uuid.getValue());
        }, () -> {
            return findBank().presets.getValue().size() == 3;
        });

        setStepName("Create Presets");
    }

    private Bank findBank() {
        return Banks.get().findByName(bankName);
    }

    private void deleteBank() {
        addStep(() -> {
            Bank b = findBank();
            server.deleteBank(b.uuid.getValue());
        }, () -> {
            return findBank() == null;
        });

        setStepName("Delete Bank");
    }
}
