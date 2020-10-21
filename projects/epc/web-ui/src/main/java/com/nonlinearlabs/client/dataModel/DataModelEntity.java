package com.nonlinearlabs.client.dataModel;

public class DataModelEntity<T> extends Notifier<T> implements DataModelEntityBase {
	private T value;

	public DataModelEntity(T def) {
		value = def;
	}

	public boolean setValue(T v) {
		return setValue(v, false);
	}

	public boolean setValue(T v, boolean immediate) {
		if (v == null) {
			if (value != null) {
				value = v;
				notifyChanges(immediate);
				return true;
			}
			return false;
		}

		if (!v.equals(value)) {
			value = v;
			notifyChanges(immediate);
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
