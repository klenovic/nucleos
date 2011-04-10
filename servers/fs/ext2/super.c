/* This file manages the super block structure.
 *
 * The entry points into this file are
 *   get_super:       search the 'superblock' table for a device
 *   read_super:      read a superblock
 *
 * Created (MFS based):
 *   February 2010 (Evgeniy Ivanov)
 */

#include "fs.h"
#include <nucleos/string.h>
#include <nucleos/com.h>
#include <nucleos/u64.h>
#include <nucleos/magic.h>
#include <servers/ext2/buf.h>
#include <servers/ext2/inode.h>
#include <servers/ext2/super.h>
#include <servers/ext2/const.h>

static off_t ext2_max_size(int block_size);
static u32_t ext2_count_dirs(struct ext2_super_block *sp);

static void super_copy(register struct ext2_super_block *dest,
		register struct ext2_super_block *source);
static void copy_group_descriptors(register struct group_desc *dest_array,
			    register struct group_desc *source_array,
			    unsigned int ngroups);

static off_t ext2_super_block_offset;


/*===========================================================================*
 *                              get_super                                    *
 *===========================================================================*/
struct ext2_super_block *get_super(
  dev_t dev           /* device number whose ext2_super_block is sought */
)
{
  if (dev == NO_DEV)
	panic("EXT2","request for ext2_super_block of NO_DEV",NO_NUM);
  if (superblock->s_dev != dev)
	panic("EXT2","wrong superblock", (int) dev);

  return(superblock);
}


/*===========================================================================*
 *                              get_block_size                               *
 *===========================================================================*/
unsigned int get_block_size(dev_t dev)
{
  if (dev == NO_DEV)
	panic("EXT2","request for block size of NO_DEV",NO_NUM);
  return(fs_block_size);
}

static struct group_desc *ondisk_group_descs;

/*===========================================================================*
 *                              read_super                                   *
 *===========================================================================*/
int read_super(sp)
register struct ext2_super_block *sp; /* pointer to a superblock */
{
  /* Read a superblock. */
  dev_t dev;
  int r;
  /* group descriptors, sp->s_group_desc points to this. */
  static struct group_desc *group_descs;
  char *buf;
  block_t gd_size; /* group descriptors table size in blocks */
  int gdt_position;

  dev = sp->s_dev;              /* save device (will be overwritten by copy) */
  if (dev == NO_DEV)
	panic("EXT2","request for ext2_super_block of NO_DEV",NO_NUM);

  if (opt.block_with_super == 0) {
	ext2_super_block_offset = SUPER_BLOCK_BYTES;
  } else {
	/* The block number here uses 1k units */
	ext2_super_block_offset = opt.block_with_super * 1024;
  }

  STATICINIT(ondisk_superblock, sizeof(struct ext2_super_block));

  if (!sp || !ondisk_superblock)
	panic("EXT2","can't allocate memory for ext2_super_block buffers",NO_NUM);

  r = block_dev_io(MFS_DEV_READ, dev, SELF_E,
		   (char*) ondisk_superblock, cvu64(ext2_super_block_offset),
		   _MIN_BLOCK_SIZE);
  if (r != _MIN_BLOCK_SIZE)
	return(-EINVAL);

  super_copy(sp, ondisk_superblock);

  sp->s_dev = NO_DEV;           /* restore later */

  if (sp->s_magic != EXT2_SUPER_MAGIC)
	return(-EINVAL);

  sp->s_block_size = 1024*(1<<sp->s_log_block_size);

  if (sp->s_block_size < _MIN_BLOCK_SIZE
      || sp->s_block_size >_MAX_BLOCK_SIZE) {
	return(-EINVAL);
	printk("data block size is too large\n");
  }

  if ((sp->s_block_size % 512) != 0)
	return(-EINVAL);

  if (SUPER_SIZE_D > sp->s_block_size)
	return(-EINVAL);

  /* Variable added for convinience (i_blocks counts 512-byte blocks). */
  sp->s_sectors_in_block = sp->s_block_size / 512;

  /* TODO: this code is for revision 1 (but bw compatible with 0)
   * inode must be power of 2 and smaller, than block size.
   */
  if ((EXT2_INODE_SIZE(sp) & (EXT2_INODE_SIZE(sp) - 1)) != 0
      || EXT2_INODE_SIZE(sp) > sp->s_block_size) {
	printk("superblock->s_inode_size is incorrect...\n");
	return(-EINVAL);
  }

  sp->s_blocksize_bits = sp->s_log_block_size + 10;
  sp->s_max_size = ext2_max_size(sp->s_block_size);
  sp->s_inodes_per_block = sp->s_block_size / EXT2_INODE_SIZE(sp);
  if (sp->s_inodes_per_block == 0 || sp->s_inodes_per_group == 0) {
	printk("either inodes_per_block or inodes_per_group count is 0\n");
	return(-EINVAL);
  }

  sp->s_itb_per_group = sp->s_inodes_per_group / sp->s_inodes_per_block;
  sp->s_desc_per_block = sp->s_block_size / sizeof(struct group_desc);

  sp->s_groups_count = ((sp->s_blocks_count - sp->s_first_data_block - 1)
				/ sp->s_blocks_per_group) + 1;

  /* ceil(groups_count/desc_per_block) */
  sp->s_gdb_count = (sp->s_groups_count + sp->s_desc_per_block - 1)
			/ sp->s_desc_per_block;

  gd_size = sp->s_gdb_count * sp->s_block_size;

  buf = 0;
  STATICINIT(buf, gd_size);
  group_descs = (struct group_desc *) buf;

  buf = 0;
  STATICINIT(buf, gd_size);
  ondisk_group_descs = (struct group_desc *) buf;

  if (!group_descs || !ondisk_group_descs)
	panic("EXT2","can't allocate memory for gdt buffer",NO_NUM);

  /* s_first_data_block (block number, where superblock is stored)
   * is 1 for 1Kb blocks and 0 for larger blocks.
   * For fs with 1024-byte blocks first 1024 bytes (block0) used by MBR,
   * and block1 stores superblock. When block size is larger, block0 stores
   * both MBR and superblock, but gdt lives in next block anyway.
   * If sb=N was specified, then gdt is stored in N+1 block, the block number
   * here uses 1k units.
   *
   */
  if (opt.block_with_super == 0) {
	gdt_position = (sp->s_first_data_block + 1) * sp->s_block_size;
  } else {
	gdt_position = (opt.block_with_super + 1) * 1024;
  }

  r = block_dev_io(MFS_DEV_READ, dev, SELF_E,
                   (char*) ondisk_group_descs, cvu64(gdt_position),
                   gd_size);
  if (r != gd_size) {
	printk("Can not read group descriptors\n");
	return(-EINVAL);
  }

  /* TODO: check descriptors we just read */

  copy_group_descriptors(group_descs, ondisk_group_descs, sp->s_groups_count);
  sp->s_group_desc = group_descs;

  /* Make a few basic checks to see if super block looks reasonable. */
  if (sp->s_inodes_count < 1 || sp->s_blocks_count < 1) {
	printk("not enough inodes or data blocks, \n");
	return(-EINVAL);
  }

  sp->s_dirs_counter = ext2_count_dirs(sp);

  /* Start block search from this block.
   * We skip superblock (1 block), group descriptors blocks (sp->s_gdb_count)
   * block and inode bitmaps (2 blocks) and inode table.
   */
  sp->s_bsearch = sp->s_first_data_block + 1 + sp->s_gdb_count + 2
			+ sp->s_itb_per_group;

  sp->s_igsearch = 0;

  sp->s_dev = dev; /* restore device number */
  return(0);
}


/*===========================================================================*
 *                              write_super				     *
 *===========================================================================*/
void write_super(sp)
struct ext2_super_block *sp; /* pointer to a superblock */
{
/* Write a superblock and gdt. */
  int r;
  block_t gd_size; /* group descriptors table size in blocks */
  int gdt_position;

  if (sp->s_rd_only)
	panic("EXT2","can't write superblock on read-only filesys.",NO_NUM);

  if (sp->s_dev == NO_DEV)
	panic("EXT2","request to write ext2_super_block, but NO_DEV",NO_NUM);

  super_copy(ondisk_superblock, sp);

  r = block_dev_io(MFS_DEV_WRITE, sp->s_dev, SELF_E,
                   sp, cvu64(ext2_super_block_offset), SUPER_SIZE_D);
  if (r != SUPER_SIZE_D)
	printk("ext2: Warning, failed to write superblock to the disk!\n");

  if (group_descriptors_dirty == DIRTY) {
	/* Locate the appropriate ext2_super_block. */
	gd_size = sp->s_gdb_count * sp->s_block_size;

	if (opt.block_with_super == 0) {
		gdt_position = (sp->s_first_data_block + 1) * sp->s_block_size;
	} else {
		gdt_position = (opt.block_with_super + 1) * 1024;
	}

        copy_group_descriptors(ondisk_group_descs, sp->s_group_desc,
			       sp->s_groups_count);

	r = block_dev_io(MFS_DEV_WRITE, sp->s_dev, SELF_E,
			 (char*) ondisk_group_descs, cvu64(gdt_position),
			 gd_size);
	if (r != gd_size) {
		printk("Can not write group descriptors\n");
	}
	group_descriptors_dirty = CLEAN;
  }
}


/*===========================================================================*
 *                              get_group_desc                               *
 *===========================================================================*/
struct group_desc* get_group_desc(unsigned int bnum)
{
  if (bnum >= superblock->s_groups_count) {
	printk("ext2, get_group_desc: wrong bnum (%d) requested\n", bnum);
	return NULL;
  }
  return &superblock->s_group_desc[bnum];
}


static u32_t ext2_count_dirs(struct ext2_super_block *sp)
{
  u32_t count = 0;
  int i;

  for (i = 0; i < sp->s_groups_count; i++) {
	struct group_desc *desc = get_group_desc(i);
	if (!desc)
		continue; /* TODO: fail? */
	count += desc->used_dirs_count;
  }
  return count;
}


/*===========================================================================*
 *                              ext2_max_size                                *
 *===========================================================================*/
/* There are several things, which affect max filesize:
 * - inode.i_blocks (512-byte blocks) is limited to (2^32 - 1).
 * - number of addressed direct, single, double and triple indirect blocks.
 * Number of addressed blocks depends on block_size only, thus unlike in
 * linux (ext2_max_size) we do not make calculations, but use constants
 * for different block sizes. Calculations (gcc code) are commented.
 * Note: linux ext2_max_size makes calculated based on shifting, not
 * arithmetics.
 * (!!!)Note: constants hardly tight to EXT2_NDIR_BLOCKS, but I doubt its value
 * will be changed someday. So if it's changed, then just recalculate constatns.
 * Anyway this function is safe for any change.
 * Note: there is also limitation from VFS (to LONG_MAX, i.e. 2GB).
 */
static off_t ext2_max_size(int block_size)
{
  /* 12 is EXT2_NDIR_BLOCKS used in calculations. */
  if (EXT2_NDIR_BLOCKS != 12)
	panic("EXT2","ext2_max_size needs modification!",NO_NUM);
  switch(block_size) {
	case 1024: return LONG_MAX; /* actually 17247252480 */
	case 2048: return LONG_MAX; /* 275415851008 */
	case 4096: return LONG_MAX; /* 2194719883264 */
	default: {
		printk("ext2_max_size: Unsupported block_size! \
				Assuming bs is 1024 bytes\n");
		return 67383296L;
	}
  }
#if 0
  long addr_in_block = block_size/4; /* 4 bytes per addr */
  long sectors_in_block = block_size/512;
  long long meta_blocks; /* single, double and triple indirect blocks */
  unsigned long long out_range_s; /* max blocks addressed by inode */
  unsigned long long max_bytes;
  unsigned long long upper_limit;

  /* 1 indirect block, 1 + addr_in_block dindirect and 1 + addr_in_block +
   * + addr_in_block*addr_in_block triple indirect blocks */
  meta_blocks = 2*addr_in_block + addr_in_block*addr_in_block + 3;
  out_range_s = EXT2_NDIR_BLOCKS + addr_in_block + addr_in_block * addr_in_block
                + addr_in_block * addr_in_block * addr_in_block;
  max_bytes = out_range_s * block_size;

  upper_limit = (1LL << 32) - 1; /* max 512-byte blocks by i_blocks */
  upper_limit /= sectors_in_block; /* total block_size blocks */
  upper_limit -= meta_blocks; /* total data blocks */
  upper_limit *= (long long)block_size; /* max size in bytes */

  if (max_bytes > upper_limit)
	max_bytes = upper_limit;

  /* Limit s_max_size to LONG_MAX */
  if (max_bytes > LONG_MAX)
	max_bytes = LONG_MAX;

  return max_bytes;
#endif
}


/*===========================================================================*
 *				super_copy				     *
 *===========================================================================*/
static void super_copy(
  register struct ext2_super_block *dest,
  register struct ext2_super_block *source
)
/* Note: we don't convert stuff, used in ext3. */
{
/* Copy ext2_super_block to the in-core table, swapping bytes if need be. */
  if (le_CPU) {
	/* Just use memcpy */
	memcpy(dest, source, SUPER_SIZE_D);
	return;
  }
  dest->s_inodes_count = conv4(le_CPU, source->s_inodes_count);
  dest->s_blocks_count = conv4(le_CPU, source->s_blocks_count);
  dest->s_r_blocks_count = conv4(le_CPU, source->s_r_blocks_count);
  dest->s_free_blocks_count = conv4(le_CPU, source->s_free_blocks_count);
  dest->s_free_inodes_count = conv4(le_CPU, source->s_free_inodes_count);
  dest->s_first_data_block = conv4(le_CPU, source->s_first_data_block);
  dest->s_log_block_size = conv4(le_CPU, source->s_log_block_size);
  dest->s_log_frag_size = conv4(le_CPU, source->s_log_frag_size);
  dest->s_blocks_per_group = conv4(le_CPU, source->s_blocks_per_group);
  dest->s_frags_per_group = conv4(le_CPU, source->s_frags_per_group);
  dest->s_inodes_per_group = conv4(le_CPU, source->s_inodes_per_group);
  dest->s_mtime = conv4(le_CPU, source->s_mtime);
  dest->s_wtime = conv4(le_CPU, source->s_wtime);
  dest->s_mnt_count = conv2(le_CPU, source->s_mnt_count);
  dest->s_max_mnt_count = conv2(le_CPU, source->s_max_mnt_count);
  dest->s_magic = conv2(le_CPU, source->s_magic);
  dest->s_state = conv2(le_CPU, source->s_state);
  dest->s_errors = conv2(le_CPU, source->s_errors);
  dest->s_minor_rev_level = conv2(le_CPU, source->s_minor_rev_level);
  dest->s_lastcheck = conv4(le_CPU, source->s_lastcheck);
  dest->s_checkinterval = conv4(le_CPU, source->s_checkinterval);
  dest->s_creator_os = conv4(le_CPU, source->s_creator_os);
  dest->s_rev_level = conv4(le_CPU, source->s_rev_level);
  dest->s_def_resuid = conv2(le_CPU, source->s_def_resuid);
  dest->s_def_resgid = conv2(le_CPU, source->s_def_resgid);
  dest->s_first_ino = conv4(le_CPU, source->s_first_ino);
  dest->s_inode_size = conv2(le_CPU, source->s_inode_size);
  dest->s_block_group_nr = conv2(le_CPU, source->s_block_group_nr);
  dest->s_feature_compat = conv4(le_CPU, source->s_feature_compat);
  dest->s_feature_incompat = conv4(le_CPU, source->s_feature_incompat);
  dest->s_feature_ro_compat = conv4(le_CPU, source->s_feature_ro_compat);
  memcpy(dest->s_uuid, source->s_uuid, sizeof(dest->s_uuid));
  memcpy(dest->s_volume_name, source->s_volume_name,
	 sizeof(dest->s_volume_name));
  memcpy(dest->s_last_mounted, source->s_last_mounted,
	 sizeof(dest->s_last_mounted));
  dest->s_algorithm_usage_bitmap =
			conv4(le_CPU, source->s_algorithm_usage_bitmap);
  dest->s_prealloc_blocks = source->s_prealloc_blocks;
  dest->s_prealloc_dir_blocks = source->s_prealloc_dir_blocks;
  dest->s_padding1 = conv2(le_CPU, source->s_padding1);
}


/*===========================================================================*
 *				gd_copy 				     *
 *===========================================================================*/
static void gd_copy(
  register struct group_desc *dest,
  register struct group_desc *source
)
{
  /* Copy ext2_super_block to the in-core table, swapping bytes if need be. */
  if (le_CPU) {
	/* Just use memcpy */
	memcpy(dest, source, sizeof(struct group_desc));
	return;
  }
  dest->block_bitmap = conv4(le_CPU, source->block_bitmap);
  dest->inode_bitmap = conv4(le_CPU, source->inode_bitmap);
  dest->inode_table = conv4(le_CPU, source->inode_table);
  dest->free_blocks_count = conv2(le_CPU, source->free_blocks_count);
  dest->free_inodes_count = conv2(le_CPU, source->free_inodes_count);
  dest->used_dirs_count = conv2(le_CPU, source->used_dirs_count);
}


/*===========================================================================*
 *			copy_group_descriptors  			     *
 *===========================================================================*/
static void copy_group_descriptors(
  register struct group_desc *dest_array,
  register struct group_desc *source_array,
  unsigned int ngroups
)
{
  int i;
  for (i = 0; i < ngroups; i++)
	gd_copy(&dest_array[i], &source_array[i]);
}
