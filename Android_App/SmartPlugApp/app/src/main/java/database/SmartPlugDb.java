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

    /** TODO definir las otras 2 tablas que componen la base de datos */
}
