#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/slab.h>
/**

The second part of this project involves modifying the kernel module so that it
uses the kernel linked-list data structure.


circular, doubly linked list that is available to kernel developers.

TODO: look at linux includes.

embed the linked list within the nodes that comprise the list.


**/

struct birthday {
  int day;
  int month;
  int year;
  struct list_head list; // #include <linux/types.h> // embed the linked list within the nodes that comprise the list.
  // By embedding the linked list within the structure, Linux makes it
  //possible to manage the data structure with a series of macro functions.
};

/**

We can declare a list head object, which we use as a reference to the head
of the list by using the LIST_HEAD() macro

**/

static LIST_HEAD(birthday_list); // variable birthday list, which is of type struct list_head.

// test edit.
// test 2
/* This function is called when the module is loaded. */
int simple_init(void)
{
  printk(KERN_INFO "Loading Module yo lo.\n");

  struct birthday *person;

  // create a new node.

  person = kmalloc(sizeof(*person), GFP_KERNEL);
  person->day = 15;
  person->month= 2;
  person->year = 1988;
  INIT_LIST_HEAD(&person->list);

  list_add_tail(&person->list, &birthday_list);

  // add another node.
  person = kmalloc(sizeof(*person), GFP_KERNEL);
  person->day = 20;
  person->month= 5;
  person->year = 1961;

  INIT_LIST_HEAD(&person->list);

  list_add_tail(&person->list, &birthday_list);

/**

Traversing the list involves using the list for each entry() Macro, which accepts three parameters:
• A pointer to the structure being iterated over
• A pointer to the head of the list being iterated over
• The name of the variable containing the list head structure

**/
  struct birthday *ptr;

  list_for_each_entry(ptr, &birthday_list, list) {
  /* on each iteration ptr points */
  /* to the next birthday struct */
    if(ptr)
      printk(KERN_INFO "%d %d %d - :).\n", ptr->day, ptr->month, ptr->year);

  }

  return 0;
}

/* This function is called when the module is removed. */
void simple_exit(void) {
	printk(KERN_INFO "Removing Module son.\n");

  struct birthday *ptr, *next;

  list_for_each_entry_safe(ptr, next, &birthday_list, list) {
    /* on each iteration ptr points */
    /* to the next birthday struct */
    if(ptr) {
      printk(KERN_INFO "freeing - %d %d %d - :).\n", ptr->day, ptr->month, ptr->year);
      list_del(&ptr->list);
      kfree(ptr);
    }

  }

  printk(KERN_INFO "Done son.\n");

}

/* Macros for registering module entry and exit points. */
module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");

