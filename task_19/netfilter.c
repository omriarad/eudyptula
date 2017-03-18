
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/textsearch.h>

#define MY_ID "91bc4039c624"
#define ID_LEN 12

static struct ts_config *conf;

unsigned int eudyptula_hook(void *priv,
		struct sk_buff *skb,
		const struct nf_hook_state *state)
{
	if (skb_find_text(skb, 0, UINT_MAX, conf) != UINT_MAX)
		pr_notice("packet with %s found\n", MY_ID);

	return NF_ACCEPT;
}

static struct nf_hook_ops nfops = {
	.hook = eudyptula_hook,
	.hooknum = NF_INET_PRE_ROUTING,
	.pf = PF_INET,
	.priority = NF_IP_PRI_FILTER,
};

int __init enter_module(void)
{
	int ret = 0;

	ret = nf_register_hook(&nfops);
	if (ret) {
		pr_err("nf_register_hook failed on %d", ret);
		goto err;
	}
	conf = textsearch_prepare("kmp", MY_ID, ID_LEN,
		GFP_KERNEL, TS_AUTOLOAD);

	if (IS_ERR(conf)) {
		ret = PTR_ERR(conf);
		pr_err("textsearch_prepare failed on %d", ret);
		goto err_filter;
	}
	pr_debug("netfilter registered.\n");
	return 0;

err_filter:
	nf_unregister_hook(&nfops);
err:
	return ret;
}

void __exit exit_module(void)
{
	nf_unregister_hook(&nfops);
}

module_init(enter_module);
module_exit(exit_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("[91bc4039c624]");
MODULE_DESCRIPTION("A simple hello world kernel module");
