/*
 *    HardInfo - Displays System Information
 *    Copyright (C) 2003-2006 Leandro A. F. Pereira <leandro@linuxmag.com.br>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, version 2.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * Some code from xfce4-mount-plugin, version 0.4.3
 *  Copyright (C) 2005 Jean-Baptiste jb_dul@yahoo.com 
 *  Distributed under the terms of GNU GPL 2. 
 */
#include <sys/vfs.h>
#define KB 1024
#define MB 1048576
#define GB 1073741824

static gchar *fs_list = NULL;

static gchar *
fs_human_readable(gfloat size)
{
    if (size < KB)
	return g_strdup_printf("%.1f B", size);
    if (size < MB)
	return g_strdup_printf("%.1f KiB", size / KB);
    if (size < GB)
	return g_strdup_printf("%.1f MiB", size / MB);

    return g_strdup_printf("%.1f GiB", size / GB);
}

static void
scan_filesystems(void)
{
    FILE *mtab;
    gchar buf[1024];
    struct statfs sfs;

    g_free(fs_list);
    fs_list = g_strdup("");

    mtab = fopen("/etc/mtab", "r");
    if (!mtab)
	return;

    while (fgets(buf, 1024, mtab)) {
	gfloat size, used, avail;
	gchar **tmp;

	tmp = g_strsplit(buf, " ", 0);
	if (!statfs(tmp[1], &sfs)) {
		size = (float) sfs.f_bsize * (float) sfs.f_blocks;
		avail = (float) sfs.f_bsize * (float) sfs.f_bavail;
		used = size - avail;

		gchar *strsize = fs_human_readable(size),
		      *stravail = fs_human_readable(avail),
	  	      *strused = fs_human_readable(used);

		gchar *strhash;
		if ((strhash = g_hash_table_lookup(moreinfo, tmp[0]))) {
		    g_hash_table_remove(moreinfo, tmp[0]);
		    g_free(strhash);
		}

		strhash = g_strdup_printf("[%s]\n"
					  "Filesystem=%s\n"
					  "Mounted As=%s\n"
					  "Mount Point=%s\n"
					  "Size=%s\n"
					  "Used=%s\n"
					  "Available=%s\n",
					  tmp[0],
					  tmp[2],
					  strstr(tmp[3], "rw") ? "Read-Write" :
					  "Read-Only", tmp[1], strsize, strused,
					  stravail);
		g_hash_table_insert(moreinfo, g_strdup(tmp[0]), strhash);

		fs_list = g_strdup_printf("%s$%s$%s=%s total, %s free\n",
					  fs_list,
					  tmp[0], tmp[0], strsize, stravail);

		g_free(strsize);
		g_free(stravail);
		g_free(strused);
	}
	g_strfreev(tmp);
    }

    fclose(mtab);
}
