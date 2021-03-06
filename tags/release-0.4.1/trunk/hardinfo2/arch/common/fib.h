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
 */

static unsigned long long
fib(unsigned long long n)
{
    if (n == 0)
        return 0;
    else if (n == 1 || n == 2)
        return 1;
    return fib(n - 1) + fib(n - 2);
}

static gchar *
benchmark_fib(void)
{
    GTimer *timer = g_timer_new();
    gdouble elapsed = 0;
    
    shell_view_set_enabled(FALSE);
    shell_status_update("Calculating the 42<sup>th</sup> Fibonacci number...");
    
    g_timer_start(timer);
    fib(42);		/* the answer? :) */
    g_timer_stop(timer);

    elapsed = g_timer_elapsed(timer, NULL);
    
    g_timer_destroy(timer);

    gchar *retval = g_strdup_printf("[Results <i>(in seconds; lower is better)</i>]\n"
                           "<b>This Machine</b>=<b>%.2f</b>\n", elapsed);
    return benchmark_include_results(retval, "Fibonacci");
}

