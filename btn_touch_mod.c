/* Parts of the code have been inspired by drivers/input/apm-power.c. */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <linux/vt_kern.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Daniel Leidert");
MODULE_DESCRIPTION("A module to unblank the screen in case of a touch event.");
MODULE_VERSION("0.0.1");
//MODULE_SUPPORTED_DEVICE("input/usb-Weida_Hi-Tech_CoolTouch_System-event-if00");

static void unblank_event(unsigned int code)
{
	switch (code) {
	case BTN_TOUCH:
		pr_info("BTN_TOUCH requesting unblanking screen...\n");
		do_unblank_screen(0);
		break;
	default:
		break;
	}
}

static void btn_touch_event(struct input_handle *handle, unsigned int type,
                            unsigned int code, int value)
{
	/* only react on btn touch events */
	if (value != 1)
		return;

	switch (type) {
	case EV_KEY:
		unblank_event(code);
		break;
	default:
		break;
	}
}

static int btn_touch_connect(struct input_handler *handler,
                             struct input_dev *dev,
                             const struct input_device_id *id)
{
	struct input_handle *handle;
	int error;

	handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
	if (!handle)
		return -ENOMEM;

	handle->dev = dev;
	handle->handler = handler;
	handle->name = "btn-touch";

	error = input_register_handle(handle);
	if (error) {
		pr_err("Failed to register BTN_TOUCH handler, error %d\n", error);
		kfree(handle);
		return error;
	}

	error = input_open_device(handle);
	if (error) {
		pr_err("Failed to open BTN_TOUCH device, error %d\n", error);
		input_unregister_handle(handle);
		kfree(handle);
		return error;
	}

	return 0;
}


static void btn_touch_disconnect(struct input_handle *handle)
{
	input_close_device(handle);
	input_unregister_handle(handle);
	kfree(handle);
}

static const struct input_device_id btn_touch_ids[] = {
	{
		.flags  = INPUT_DEVICE_ID_MATCH_EVBIT | INPUT_DEVICE_ID_MATCH_ABSBIT,
		.evbit  = { BIT_MASK(EV_ABS) },
		.absbit = { BIT_MASK(ABS_X) },
	},
	{
		.flags  = INPUT_DEVICE_ID_MATCH_EVBIT | INPUT_DEVICE_ID_MATCH_ABSBIT,
		.evbit  = { BIT_MASK(EV_ABS) },
		.absbit = { BIT_MASK(ABS_Y) },
	},
	{
		.flags  = INPUT_DEVICE_ID_MATCH_EVBIT | INPUT_DEVICE_ID_MATCH_ABSBIT,
		.evbit  = { BIT_MASK(EV_ABS) },
		.absbit = { BIT_MASK(ABS_MT_TRACKING_ID) },
	},
	{
		.flags  = INPUT_DEVICE_ID_MATCH_EVBIT | INPUT_DEVICE_ID_MATCH_KEYBIT,
		.evbit  = { BIT_MASK(EV_KEY) },
		.keybit = { [BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH) },
	},
	{ },
};

MODULE_DEVICE_TABLE(input, btn_touch_ids);

static struct input_handler btn_touch_handler = {
	.event =	btn_touch_event,
	.connect =	btn_touch_connect,
	.disconnect =	btn_touch_disconnect,
	.name =		"btn-touch",
	.id_table =	btn_touch_ids,
};

static int __init btn_touch_init(void)
{
	pr_info("Watching BTN_TOUCH events.\n");
	return input_register_handler(&btn_touch_handler);
}

static void __exit btn_touch_exit(void)
{
	input_unregister_handler(&btn_touch_handler);
	pr_info("No longer watching BTN_TOUCH events.\n");
}

module_init(btn_touch_init);
module_exit(btn_touch_exit);
