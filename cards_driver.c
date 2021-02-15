/*
 * "dev/cards" minimal kernel module - /dev version
 *
 * @author Chaoneng Quan
 *
 */

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#include <linux/random.h>


/* return random number from [0,max) */
unsigned char get_random_byte(int max){
	unsigned char c;
	get_random_bytes(&c, 1);
	return c%max;
}

/* function to swap two unsigned char*/
void mySwap(unsigned char* foo, unsigned char* bar){
	unsigned char temp = *foo;
	*foo = *bar;
	*bar = temp;
}

/* shuffle the array given its name and size */
void shuffle(unsigned char * name, int size){
	int i = 0;
	unsigned char randomIndex = 0;
	for(i = 0; i < size; i++){
		randomIndex = get_random_byte(size);		//random number between [0,52)
		mySwap( &(name[i]), &(name[randomIndex]) );	//swap name[i] with name[rand]
	}
}


/*
 * hello_read is the function called when a process calls read() on
 * /dev/hello.  It writes "Hello, world!" to the buffer passed in the
 * read() call.
 */

static ssize_t cards_read(struct file * file, char * buf,
			  size_t count, loff_t *ppos)
{
	static unsigned char cardsArray[52];	//52 cards in a deck
	static int cardsDrawn = 0;
	int i = 0;

	if(*ppos ==  0){	//runs only once
		int i;
		for(i = 0; i < count; i++){
			cardsArray[i] = i;		//populate the deck in ascending order
		}
		shuffle(cardsArray, 52);		//shuffle
	}

	/*
	 * Besides copying the string to the user provided buffer,
	 * this function also checks that the user has permission to
	 * write to the buffer, that it is mapped, etc.
	 */
	for(i = 0; i < count; i++){
		if(cardsDrawn == 52){			//if user has read 52 cards, shuffle the deck
			shuffle(cardsArray,52);
			cardsDrawn = 0;			//reset number of cards drawn
		}

		if (copy_to_user(buf+i, &(cardsArray[i]), 1))		//writes to the buffer one byte at a time
			return -EINVAL;

		*ppos = *ppos + 1;			//update file pointer postion
		cardsDrawn++;				//update cards drawn
	}

	return count;					//tell user how many cards read
}

/*
 * The only file operation we care about is read.
 */

static const struct file_operations cards_fops = {
	.owner		= THIS_MODULE,
	.read		= cards_read,
	.llseek		= default_llseek,
};

static struct miscdevice cards_dev = {
	/*
	 * We don't care what minor number we end up with, so tell the
	 * kernel to just pick one.
	 */
	MISC_DYNAMIC_MINOR,
	/*
	 * Name ourselves /dev/cards
	 */
	"cards",
	/*
	 * What functions to call when a program performs file
	 * operations on the device.
	 */
	&cards_fops
};

static int __init
cards_init(void)
{
	int ret;

	/*
	 * Create the "hello" device in the /sys/class/misc directory.
	 * Udev will automatically create the /dev/hello device using
	 * the default rules.
	 */
	ret = misc_register(&cards_dev);
	if (ret)
		printk(KERN_ERR
		       "Unable to register \"cards!\" misc device\n");

	return ret;
}

module_init(cards_init);

static void __exit
cards_exit(void)
{
	misc_deregister(&cards_dev);
}

module_exit(cards_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Valerie Henson <val@nmt.edu>");
MODULE_DESCRIPTION("\"cards!\" minimal module");
MODULE_VERSION("dev");
