package com.nonlinearlabs.client.dataModel;

public class DataModelEntity<T> extends Notifier<T> implements DataModelEntityBase {
	private T value;

	public DataModelEntity(T def) {
		value = def;
	}

	public boolean setValue(T v) {
		if (v == null) {
			if (value != null) {
				value = v;
				notifyChanges();
				return true;
			}
			return false;
		}

		if (!v.equals(value)) {
			value = v;
			notifyChanges();
			return true;
		}
		return false;
	}

	@Override
	public T getValue() {
		return value;
	}

	@Override
	public void fromString(String str) {
	}
}
