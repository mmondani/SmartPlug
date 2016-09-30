package database;


public class SmartPlugDb {
    public static final class InstantaneousInfoTable {
        public static final String NAME = "instantaneousInfo";

        public static final class Cols {
            public static final String ID = "id";
            public static final String IP = "ip";
            public static final String LAST_UPDATE = "last_update";
            public static final String CONNECTION_STATE = "connection_state";
            public static final String LOAD_STATE = "load_state";
            public static final String VOLTAGE = "voltage";
            public static final String CURRENT = "current";
            public static final String POWER = "power";
            public static final String TOTAL_ENERGY = "total_energy";
        }
    }

    public static final class StaticInfoTable {
        public static final String NAME = "staticInfo";

        public static final class Cols {
            public static final String ID = "id";
            public static final String NAME = "name";
            public static final String ICON_ID = "icon_id";
        }
    }

    public static final class OnOffTimesTable {
        public static final String NAME = "onOffTimes";

        public static final class Cols {
            public static final String ID = "id";
            public static final String ENABLED_TIMES = "enabled_times";
            public static final String MONDAY_LOAD_ON_TIME = "monday_load_on_time";
            public static final String MONDAY_LOAD_OFF_TIME = "monday_load_off_tine";
            public static final String TUESDAY_LOAD_ON_TIME = "tuesday_load_on_time";
            public static final String TUESDAY_LOAD_OFF_TIME = "tuesday_load_off_tine";
            public static final String WEDNESDAY_LOAD_ON_TIME = "wednesday_load_on_time";
            public static final String WEDNESDAY_LOAD_OFF_TIME = "wednesday_load_off_tine";
            public static final String THURSDAY_LOAD_ON_TIME = "thursday_load_on_time";
            public static final String THURSDAY_LOAD_OFF_TIME = "thursday_load_off_tine";
            public static final String FRIDAY_LOAD_ON_TIME = "friday_load_on_time";
            public static final String FRIDAY_LOAD_OFF_TIME = "friday_load_off_tine";
            public static final String SATURDAY_LOAD_ON_TIME = "saturday_load_on_time";
            public static final String SATURDAY_LOAD_OFF_TIME = "saturday_load_off_tine";
            public static final String SUNDAY_LOAD_ON_TIME = "sunday_load_on_time";
            public static final String SUNDAY_LOAD_OFF_TIME = "sunday_load_off_tine";
        }
    }

    public static final class MeasurementsTable {
        public static final String NAME = "measurements";

        public static final class Cols {
            public static final String ID = "id";
            public static final String DATE = "date";
            public static final String MEASUREMENT_TYPE = "measurement_type";
            public static final String MEASUREMENTS = "measurements";
        }
    }
}
