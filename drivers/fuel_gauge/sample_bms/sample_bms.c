#define DT_DRV_COMPAT sample_bms

#include <zephyr/kernel.h>
#include <zephyr/drivers/fuel_gauge.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/logging/log.h>
#include <string.h>

LOG_MODULE_REGISTER(sample_bms);

struct sample_bms_config {
    struct i2c_dt_spec i2c;
};

static int sample_bms_get_prop(const struct device *dev, fuel_gauge_prop_t prop, union fuel_gauge_prop_val *val){
    int rc = 0;
    uint16_t tmp_val = 0;

    switch (prop) {
	case FUEL_GAUGE_AVG_CURRENT:
		val->avg_current = 1000;
		break;
	case FUEL_GAUGE_CYCLE_COUNT:
		val->cycle_count = 1;
		break;
	case FUEL_GAUGE_CURRENT:
		val->current = 1000;
		break;
	case FUEL_GAUGE_FULL_CHARGE_CAPACITY:
		val->full_charge_capacity = 1000;
		break;
	case FUEL_GAUGE_REMAINING_CAPACITY:
		val->remaining_capacity = 1000;
		break;
	case FUEL_GAUGE_RUNTIME_TO_EMPTY:
		val->runtime_to_empty = 1000;
		break;
	case FUEL_GAUGE_RUNTIME_TO_FULL:
		val->runtime_to_full = 1000;
		break;
	case FUEL_GAUGE_RELATIVE_STATE_OF_CHARGE:
		val->relative_state_of_charge = 50;
		break;
	case FUEL_GAUGE_TEMPERATURE:
		val->temperature = 30;
		break;
	case FUEL_GAUGE_VOLTAGE:
		val->voltage = 3200;
		break;
	case FUEL_GAUGE_SBS_ATRATE:
		val->sbs_at_rate = 1000;
		break;
	case FUEL_GAUGE_SBS_ATRATE_TIME_TO_EMPTY:
		val->sbs_at_rate_time_to_empty = 1000;
		break;
	case FUEL_GAUGE_CHARGE_VOLTAGE:
		val->chg_voltage = 1000;
		break;
	case FUEL_GAUGE_CHARGE_CURRENT:
		val->chg_current = 1000;
		break;
	case FUEL_GAUGE_STATUS:
		val->fg_status = 0;
		break;
	case FUEL_GAUGE_DESIGN_CAPACITY:
		val->design_cap = 1000;
		break;
	default:
		rc = -ENOTSUP;
	}

	return rc;
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static int sample_bms_get_buffer_prop(const struct device *dev, fuel_gauge_prop_t prop, void *dst, size_t dst_len){
    int rc = 0;

    switch(prop){
        case FUEL_GAUGE_MANUFACTURER_NAME:
            memcpy(dst, "SAMPLE_MFR", MIN(sizeof("SAMPLE_MFR"), dst_len));
            break;
        case FUEL_GAUGE_DEVICE_NAME:
            memcpy(dst, "SAMPLE_BMS", MIN(sizeof("SAMPLE_BMS"), dst_len));
            break;
        case FUEL_GAUGE_DEVICE_CHEMISTRY:
            memcpy(dst, "Lithium Ion", MIN(sizeof("Lithium Ion"), dst_len));
            break;
        default:
            rc = -ENOTSUP;
    }
    return rc;
}


static int sample_bms_set_prop(const struct device *dev, fuel_gauge_prop_t prop, union fuel_gauge_prop_val val){
    int rc = 0;
    switch(prop){
        case FUEL_GAUGE_SBS_MFR_ACCESS:
            val.sbs_mfr_access_word = 0;
            break;
        case FUEL_GAUGE_SBS_ATRATE:
            val.sbs_at_rate = 0;
            break;
        default:
            rc = -ENOTSUP;
    }
    return rc;
}

static int sample_bms_init(const struct device *dev)
{
    const struct sample_bms_config *config = dev->config;

    if(!device_is_ready(config->i2c.bus)) {
        LOG_ERR("Bus device is not ready");
        return -ENODEV;
    }

    return 0;
}

static const struct fuel_gauge_driver_api sample_bms_driver_api = {
    .get_property = sample_bms_get_prop,
    .set_property = sample_bms_set_prop,
    .get_buffer_property = sample_bms_get_buffer_prop,
};

#define SAMPLE_BMS_INIT(index) \
    static const struct sample_bms_config sample_bms_config_##index = { \
        .i2c =  I2C_DT_SPEC_INST_GET(index) \
    }; \
    DEVICE_DT_INST_DEFINE(index, sample_bms_init, NULL, NULL, &sample_bms_config_##index, POST_KERNEL, CONFIG_FUEL_GAUGE_INIT_PRIORITY, &sample_bms_driver_api);

DT_INST_FOREACH_STATUS_OKAY(SAMPLE_BMS_INIT)