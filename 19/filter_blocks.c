#include <linux/init.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/textsearch.h>

MODULE_AUTHOR("Lorenzo Stoakes <lstoakes@gmail.com>");
MODULE_DESCRIPTION("Monitors IPv4 for Eudyptula ID");
MODULE_LICENSE("GPL");

nf_hookfn hook_func;

static const char eudyptula_id[] = "f8cdc7cef553";
static struct nf_hook_ops hook_ops = {
	.hook = hook_func,
	.owner = THIS_MODULE,
	.pf = PF_INET,
	.hooknum = NF_INET_PRE_ROUTING,
	.priority = NF_IP_PRI_FIRST
};
static struct ts_config *id_search_config;

unsigned int hook_func(const struct nf_hook_ops *ops,
		struct sk_buff *skb,
		const struct net_device *in,
		const struct net_device *out,
		int (*okfn)(struct sk_buff *))
{
	int pos;
	struct ts_state state;

	pos = skb_find_text(skb, 0, INT_MAX, id_search_config, &state);
	for (; pos >= 0; pos = textsearch_next(id_search_config, &state))
		pr_debug(eudyptula_id);

	return NF_ACCEPT;
}

static int __init filter_init(void)
{
	id_search_config = textsearch_prepare("kmp", eudyptula_id, 12,
					GFP_KERNEL, TS_AUTOLOAD);

	nf_register_hook(&hook_ops);

	return 0;
}

static void __exit filter_exit(void)
{
	nf_unregister_hook(&hook_ops);
	kfree(id_search_config);
}

module_init(filter_init);
module_exit(filter_exit);
